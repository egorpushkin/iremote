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

#ifndef FBMANAGER_H__SFB__REMOTEPC__INCLUDED_
#define FBMANAGER_H__SFB__REMOTEPC__INCLUDED_

namespace RemotePC
{
namespace SFB
{

    class FBManager
        : public mc::CommonImpl< mc::IRunnable >
    {
    public:

        FBManager(IServicesManagerRef servicesManager, IServiceHandlerRef serviceHandler);
        ~FBManager();

        bool configure(float fps, const RasterFormatPtr& format);

        void start();

        void stop();

        /**
         * IRunnable section
         */

        virtual mc::result Run();

    protected:

        /**
         * FB logic section
         */

        void update();

        void init();

        void deliver();

        void beforeUpdate();

        void delay();

        /**
         * Helpers section
         */

        mc::longlong fpsToDuration(float fps) const;

    private:

        IServicesManagerWeak servicesManager_;

        IServiceHandlerWeak serviceHandler_;

        mc::IThreadPtr thread_;

        bool cancelled_;

        mc::longlong frameDuration_;

        mc::longlong frameStart_;

        MemoryCachePtr cache_;

        ImageSource imageSource_;

        RasterFormatPtr rasterFormat_;

        SwapBuffer swapBuffer_;

        MatrixComparator matrixComparator_;

        DeliveryQueuePtr deliveryQueue_;

        MatrixPtr matrixToDeliver_;

    };
}
}

#endif // FBMANAGER_H__SFB__REMOTEPC__INCLUDED_
