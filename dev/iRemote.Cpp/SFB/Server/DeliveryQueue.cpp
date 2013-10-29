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

#include "../Messages/BlockMessage.h"
#include "../Messages/BatchHeader.h"
#include "Messages/MoveMessage.h"
#include "Messages/MoveToMessage.h"
#include "../Hardware/Hardware.h"

#ifdef WIN32
#undef max
#endif

namespace MinCOM
{
    // {56CB6EB0-BF19-46B5-A380-725EA8CAA07D}
    template<> const Guid TypeInfo< RemotePC::SFB::DeliveryQueue >::iid_ =
        { 0x56CB6EB0, 0xBF19, 0x46B5, { 0xA3, 0x80, 0x72, 0x5E, 0xA8, 0xCA, 0xA0, 0x7D } };
}

namespace RemotePC
{
namespace SFB
{
    DeliveryQueue::DeliveryQueue()
        : mc::CommonImpl< mc::IRunnable >()
        , thread_()
        , lock_( mc::Library::Semaphore(0, std::numeric_limits< long >::max()) )
        , queue_()
        , cancelled_( false )
        , compressor_()
    {
    }

    void DeliveryQueue::start()
    {
        cancelled_ = false;

        thread_ = mc::Library::Thread();
        thread_->SetContext(GetSelf());
        thread_->Start();
    }

    void DeliveryQueue::stop()
    {
        cancelled_ = true;

        lock_->Release(1);
        thread_->Join();
        thread_ = NULL;
    }

    void DeliveryQueue::deliver(mc::IProtocolRef client, const MatrixPtr& matrix, bool fullscreen)
    {
        if ( ( NULL == matrix ) || ( ( 0 == matrix->changes().size() ) && !fullscreen ) )
            return;

        mc::CoreMutexLock lock(mc::CommonImpl< IRunnable >::GetLock());

        // IMPORTANT. Current implementation does not support multiuser mode!!!
        // In multiuser mode there should be separate queue for each client.

        if ( 0 == queue_.size() )
        {
            // Add new item to the queue.
            DeliveryPtr_ delivery(new Delivery_());
            delivery->client_ = client;
            delivery->matrix_ = matrix;
            delivery->fullscreen_ = fullscreen;
            queue_.push(delivery);
            lock_->Release(1);
        }
        else
        {            
            DeliveryPtr_ delivery = queue_.front();
            // Merging is only required, if fullscreen is off for bot new and old
            // matrixes. Otherwise, all cells are sent in any case.
            if ( !delivery->fullscreen_ && !fullscreen )
            {
                // Copy modified cells from existing to new matrix.
                // We will store new matrix locally, as it refers to newer screenshot.
                Matrix::ChangesList_& changes = delivery->matrix_->changes();
                Matrix::ChangesList_::const_iterator iter = changes.begin();
                Matrix::ChangesList_::const_iterator end = changes.end();
                for ( ; iter != end ; ++iter )
                {
                    const Matrix::CellLocation_& location = *iter;
                    if ( !matrix->cells()[location.y_][location.x_].modified_ )
                    {
                        matrix->cells()[location.y_][location.x_].modified_ = true;
                        matrix->changes().push_back(Matrix::CellLocation_(location.x_, location.y_));
                    }
                }
            }
            delivery->fullscreen_ = ( delivery->fullscreen_ || fullscreen );
            // Update matrix properties.
            delivery->matrix_ = matrix;
        }
    }

    void DeliveryQueue::send(const DeliveryPtr_& delivery)
    {
        if ( !delivery )
            return;

        // Chose proper list to deliver.
        Matrix::ChangesList_& changes
            = ( delivery->fullscreen_ )
            ? ( delivery->matrix_->allCells() )
            : ( delivery->matrix_->changes() );

        // Skip empty deliverables.
        if ( 0 == changes.size() )
            return;

        // Send mouse position.
        ScreenPoint position = HardwareProvider::Instance().GetMouseControl()->GetPosition();
        mc::IMessagePtr positionMessage( mc::Class< MoveToMessage >::Create(
            position.x_, position.y_) );
        delivery->client_->Send(positionMessage);

        // Send batch message with a number of cells.
        mc::IMessagePtr header( mc::Class< BatchHeader >::Create(
            mc::TypeInfo< mc::ICommon >::GetGuid(),
            changes.size() ) );
        delivery->client_->Send(header);

        // Walk through the list, compress and deliver each cell.
        Matrix::Cell_ **cells = delivery->matrix_->cells();
        Matrix::ChangesList_::const_iterator iter = changes.begin();
        Matrix::ChangesList_::const_iterator end = changes.end();
        for ( ; iter != end ; ++iter )
        {
            const Matrix::CellLocation_& location = *iter;
            Matrix::Cell_& cell = cells[location.y_][location.x_];

            // Compress cell, if it is not yet done.
            if ( ByteBufferPtr() == cell.buffer_ )
            {
                cell.buffer_ = compressor_.compress(delivery->matrix_->image(), cell.x_, cell.y_, cell.width_, cell.height_);
            }

            // Send cell data.
            mc::IMessagePtr block( mc::Class< BlockMessage >::Create(
                (float)cell.x_, (float)cell.y_, (float)cell.width_, (float)cell.height_,
                cell.buffer_->length(), (const char *)cell.buffer_->data() ) );
            delivery->client_->Send(block);
        }
    }

    // IRunnable section
    mc::result DeliveryQueue::Run()
    {
        while ( !cancelled_ )
        {
            // Wait until any job arrives.
            if ( mc::Error::IsFailed(lock_->Wait()) )
                return mc::_E_FAIL;

            if ( cancelled_ )
                break;

            DeliveryPtr_ delivery = pop();
            if ( !delivery )
                continue;

            send(delivery);
        }

        return mc::_S_OK;
    }

    DeliveryQueue::DeliveryPtr_ DeliveryQueue::pop()
    {
        mc::CoreMutexLock lock(mc::CommonImpl< IRunnable >::GetLock());

        if ( queue_.size() == 0 )
            return DeliveryPtr_();

        DeliveryPtr_ delivery = queue_.front();
        queue_.pop();

        return delivery;
    }
}
}
