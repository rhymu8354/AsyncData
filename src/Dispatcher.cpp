/**
 * @file Dispatcher.cpp
 *
 * This module contains the implementation of the
 * AsyncData::Dispatcher class.
 *
 * © 2015-2018 by Richard Walters
 */

#include <AsyncData/Dispatcher.hpp>
#include <AsyncData/MultiProducerMultiConsumerStack.hpp>
#include <AsyncData/MultiProducerSingleConsumerQueue.hpp>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <thread>

namespace AsyncData {

    /**
     * This holds the private properties of the Dispatcher class.
     */
    struct Dispatcher::Impl {
        /**
         * @todo Needs documentation
         */
        MultiProducerSingleConsumerQueue< std::shared_ptr< TaskWrapper > > tasksToBeDone;

        /**
         * @todo Needs documentation
         */
        MultiProducerMultiConsumerStack< std::shared_ptr< TaskWrapper > > recycledTaskWrappers;

        /**
         * @todo Needs documentation
         */
        std::thread dispatcher;

        /**
         * @todo Needs documentation
         */
        bool stop = false;

        /**
         * @todo Needs documentation
         */
        std::mutex dispatchMutex;

        /**
         * @todo Needs documentation
         */
        std::condition_variable dispatchCondition;
    };

    Dispatcher::~Dispatcher() noexcept {
        Stop();
    }
    Dispatcher::Dispatcher(Dispatcher&& other) noexcept = default;
    Dispatcher& Dispatcher::operator=(Dispatcher&& other) noexcept = default;

    Dispatcher::Dispatcher()
        : impl_(new Impl())
    {
        impl_->dispatcher = std::thread(
            [this]{
                std::unique_lock< decltype(impl_->dispatchMutex) > dispatchLock(impl_->dispatchMutex);
                while (
                    !impl_->stop
                    || !impl_->tasksToBeDone.IsEmpty()
                ) {
                    if (impl_->tasksToBeDone.IsEmpty()) {
                        impl_->dispatchCondition.wait(dispatchLock);
                    }
                    dispatchLock.unlock();
                    while (!impl_->tasksToBeDone.IsEmpty()) {
                        std::shared_ptr< TaskWrapper > wrapper = impl_->tasksToBeDone.Remove();
                        wrapper->f();
                        if (wrapper->hasWaiter) {
                            wrapper->done = true;
                            {
                                std::lock_guard< std::mutex > doneLock(wrapper->doneMutex);
                                wrapper->doneCondition.notify_one();
                            }
                        } else {
                            wrapper->f = nullptr;
                            impl_->recycledTaskWrappers.Add(wrapper);
                        }
                    }
                    dispatchLock.lock();
                }
            }
        );
    }

    void Dispatcher::Stop() {
        if (impl_->dispatcher.joinable()) {
            {
                std::lock_guard< std::mutex > dispatchLock(impl_->dispatchMutex);
                impl_->stop = true;
                impl_->dispatchCondition.notify_one();
            }
            impl_->dispatcher.join();
        }
    }

    void Dispatcher::Post(std::function< void() > task) {
        std::shared_ptr< TaskWrapper > wrapper;
        if (!impl_->recycledTaskWrappers.Remove(wrapper)) {
            wrapper = std::make_shared< TaskWrapper >();
        }
        wrapper->hasWaiter = false;
        wrapper->f = task;
        impl_->tasksToBeDone.Add(wrapper);
        if (std::this_thread::get_id() != impl_->dispatcher.get_id()) {
            std::lock_guard< std::mutex > dispatchLock(impl_->dispatchMutex);
            impl_->dispatchCondition.notify_one();
        }
    }

}
