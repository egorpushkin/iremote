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

#ifndef DELIVERYQUEUE_H__SFB__REMOTEPC__INCLUDED_
#define DELIVERYQUEUE_H__SFB__REMOTEPC__INCLUDED_

namespace RemotePC
{
namespace SFB
{
    /**
     * Current implementation supports single connected device only.
     */
    class DeliveryQueue
        : public mc::CommonImpl< mc::IRunnable >
    {
    public:

        DeliveryQueue();

        void start();

        void stop();

        void deliver(mc::IProtocolRef client, const MatrixPtr& matrix, bool fullscreen);

        /**
         * IRunnable section
         */

        virtual mc::result Run();

    protected:

        typedef struct tagDelivery_
        {
            mc::IProtocolPtr client_;
            MatrixPtr matrix_;
            bool fullscreen_;
        }
        Delivery_;

        typedef boost::shared_ptr< Delivery_ > DeliveryPtr_;

        void send(const DeliveryPtr_& delivery);

        DeliveryPtr_ pop();

    private:

        mc::IThreadPtr thread_;

        mc::ISemaphorePtr lock_;

        std::queue< DeliveryPtr_ > queue_;

        bool cancelled_;

        JpegCompressor compressor_;

    };

    typedef mc::Strong< DeliveryQueue > DeliveryQueuePtr;
}
}

#endif // DELIVERYQUEUE_H__SFB__REMOTEPC__INCLUDED_
