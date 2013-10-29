/**
 * Copyright (c) 2013 Egor Pushkin. All rights reserved.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "Common/Common.h"

#include "../Server/Services/Services.h"
#include "SFB.h"

#include "InitHandler.h"
#include "DeliveryHandler.h"

namespace MinCOM
{
    // {F19C8257-AF51-4418-8CA1-F32FC8A66CE4}
    template<> const Guid TypeInfo< RemotePC::SFB::FBManager >::iid_ =
        { 0xF19C8257, 0xAF51, 0x4418, { 0x8C, 0xA1, 0xF3, 0x2F, 0xC8, 0xA6, 0x6C, 0xE4 } };
}

namespace RemotePC
{
namespace SFB
{
    FBManager::FBManager(IServicesManagerRef servicesManager, IServiceHandlerRef serviceHandler)
        : mc::CommonImpl< mc::IRunnable >()
        , servicesManager_( servicesManager )
        , serviceHandler_( serviceHandler )
        , thread_()
        , cancelled_( false )
        , frameDuration_()
        , frameStart_()
        , cache_( new MemoryCache() )
        , imageSource_()
        , rasterFormat_()
        , swapBuffer_()
        , matrixComparator_()
        , deliveryQueue_( mc::Class< DeliveryQueue >::Create() )
        , matrixToDeliver_()
    {
	}

    FBManager::~FBManager()
    {
    }

    bool FBManager::configure(float fps, const RasterFormatPtr& format)
    {
        // Silently apply new fps value.
        frameDuration_ = fpsToDuration(fps);
        // Check whether service should be restarted.
        bool restart
            = ( RasterFormatPtr() == rasterFormat_ )
            || ( rasterFormat_->cellSize_ != format->cellSize_ )
            || ( rasterFormat_->format_ != format->format_ );
        // Restart service and apply new settings.
        {
            mc::CoreMutexLock locker( mc::CommonImpl< IRunnable >::GetLock() );
            restart = restart && thread_;
            if ( restart )
            {
                stop();
                rasterFormat_ = format;
                start();
            }
            else
            {
                rasterFormat_ = format;
            }
        }
        // Spread initialization message, if service was restarted.
        if ( restart )
        {
            init();
        }
        return restart;
    }

    void FBManager::start()
    {
        mc::CoreMutexLock locker( mc::CommonImpl< IRunnable >::GetLock() );

        if ( thread_ )
            return;

        deliveryQueue_->start();
        cancelled_ = false;
        thread_ = mc::Library::Thread();
        thread_->SetContext(GetSelf());
        thread_->Start();
    }

    void FBManager::stop()
    {
        mc::CoreMutexLock locker( mc::CommonImpl< IRunnable >::GetLock() );

        if ( !thread_ )
            return;

        deliveryQueue_->stop();
        cancelled_ = true;
        thread_->Join();
        thread_ = NULL;
        swapBuffer_ = SwapBuffer();
    }

    /**
     * IRunnable section
     */

    mc::result FBManager::Run()
    {
        while ( !cancelled_ )
        {
            beforeUpdate();
            update();
            if ( cancelled_ )
                break;
            deliver();
            if ( cancelled_ )
                break;
            delay();
        }
        return mc::_S_OK;
    }

    /**
     * FB logic section
     */

    void FBManager::update()
    {
        // Capture next screenshot.
        RawImagePtr image = imageSource_.nextImage(RasterFormatPtr(new RasterFormat(*rasterFormat_)), cache_);

        // Wrap image into matrix.
        MatrixPtr matrix = MatrixPtr(new Matrix(image,
            image->format()->matrixWidth_, image->format()->matrixHeight_));

        // Remember matrix for comparison.
        swapBuffer_.setNext(matrix);

        // Check whether previous matrix exists.
        if ( swapBuffer_.current() != NULL )
        {
            // Detect screen size change.
            bool changed = ( swapBuffer_.current()->image()->format()->originalWidth_
                != swapBuffer_.next()->image()->format()->originalWidth_ )
                || ( swapBuffer_.current()->image()->format()->originalHeight_
                != swapBuffer_.next()->image()->format()->originalHeight_ );
            if ( changed )
            {
                // Spread initialization message.
                init();

                // Create new cache to store image buffers.
                // Buffer allocated to store current screenshot still belongs to old cache.
                // Old cache object is removed, when all dependent matrix objects are removed.
                cache_ = MemoryCachePtr(new MemoryCache());
                // Alternative solution is to clean up old cache, which is more complex and
                // thus less stable.

                // Nothing else to be done here. Matrixes are swapped automatically,
                // so previous matrix is cleared during swap as usual below.
            }
            else
            {
                // Compare screenshots.
                matrixComparator_.compare(swapBuffer_.current(), swapBuffer_.next());

                // Check for changes prevents full screen updates, so it is omitted.
                // Delivery queue tracks, whether chenges are made itself.
                // if ( swapBuffer_.next()->isChanged() )
                {
                    // Schedule current matrix for delivery.
                    matrixToDeliver_ = swapBuffer_.next();
                }
            }
        }

        // Swap matrixes.
        swapBuffer_.swap();
    }

    void FBManager::init()
    {
        IServicesManagerPtr servicesManager(servicesManager_);
        if ( servicesManager )
        {
            servicesManager->Handle( mc::Class< InitHandler >::Create( serviceHandler_ ) );
        }
    }

    void FBManager::deliver()
    {
        if ( MatrixPtr() == matrixToDeliver_ )
            return;
        IServicesManagerPtr servicesManager(servicesManager_);
        if ( servicesManager )
        {
            servicesManager->Handle( mc::Class< DeliveryHandler >::Create( serviceHandler_, deliveryQueue_, matrixToDeliver_ ) );
        }
        matrixToDeliver_ = MatrixPtr();
    }

    void FBManager::beforeUpdate()
    {
        // Remember frame start timestamp.
        frameStart_ = mc::IThread::Time();
    }

    void FBManager::delay()
    {
        // Get current time.
        mc::longlong frameEnd = mc::IThread::Time();
        // Specify minimal delay of 5 milliseconds.
        long minDelay = 5;
        // Calculate additional delay between frames.
        long currentDuration = (long)( frameEnd - frameStart_ );
        long currentDelay = (long)( frameDuration_ - currentDuration );
        if ( currentDelay < minDelay )
            currentDelay = minDelay;
        // Spend remaining time waiting.
        mc::IThread::Sleep(currentDelay);
    }

    /**
     * Helpers section
     */

    mc::longlong FBManager::fpsToDuration(float fps) const
    {
        double duration = 1000.0 / (double)fps;
        return (mc::longlong)duration;
    }
}
}
