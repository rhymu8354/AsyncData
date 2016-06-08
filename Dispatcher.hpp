#ifndef DATA_STRUCTURES_DISPATCHER_HPP
#define DATA_STRUCTURES_DISPATCHER_HPP

/**
 * @file Dispatcher.hpp
 *
 * This module declares the DataStructures::Dispatcher class.
 *
 * Copyright (c) 2015-2016 by Richard Walters
 */

#include "MultiProducerSingleConsumerQueue.hpp"
#include "MultiProducerMultiConsumerStack.hpp"

#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>

namespace DataStructures {

    /**
     * @todo Needs documentation
     */
    class Dispatcher {
        // Custom types
    public:
        /**
         * @todo Needs documentation
         */
        struct TaskWrapper {
            /**
             * @todo Needs documentation
             */
            std::function< void() > f;

            /**
             * @todo Needs documentation
             */
            bool hasWaiter = false;

            /**
             * @todo Needs documentation
             */
            std::mutex doneMutex;

            /**
             * @todo Needs documentation
             */
            std::condition_variable doneCondition;

            /**
             * @todo Needs documentation
             */
            bool done = false;
        };

        // Public methods
    public:
        Dispatcher() {
            _dispatcher = std::move(
                std::thread(
                    [this]{
                        std::unique_lock< std::mutex > dispatchLock(_dispatchMutex);
                        while (
                            !_stop
                            || !_tasksToBeDone.IsEmpty()
                        ) {
                            if (_tasksToBeDone.IsEmpty()) {
                                _dispatchCondition.wait(dispatchLock);
                            }
                            dispatchLock.unlock();
                            while (!_tasksToBeDone.IsEmpty()) {
                                std::shared_ptr< TaskWrapper > wrapper = _tasksToBeDone.Remove();
                                wrapper->f();
                                if (wrapper->hasWaiter) {
                                    wrapper->done = true;
                                    {
                                        std::lock_guard< std::mutex > doneLock(wrapper->doneMutex);
                                        wrapper->doneCondition.notify_one();
                                    }
                                } else {
                                    RecycleTaskWrapper(wrapper);
                                }
                            }
                            dispatchLock.lock();
                        }
                    }
                )
            );
        }

        /**
         * @todo Needs documentation
         */
        ~Dispatcher() {
            Stop();
        }

        /**
         * @todo Needs documentation
         */
        void Stop() {
            if (_dispatcher.joinable()) {
                {
                    std::lock_guard< std::mutex > dispatchLock(_dispatchMutex);
                    _stop = true;
                    _dispatchCondition.notify_one();
                }
                _dispatcher.join();
            }
        }

        /**
         * @todo Needs documentation
         */
        void Post(std::function< void() > task) {
            if (std::this_thread::get_id() == _dispatcher.get_id()) {
                task();
            } else {
                std::shared_ptr< TaskWrapper > wrapper = GetTaskWrapper();
                wrapper->hasWaiter = false;
                Post(task, wrapper);
            }
        }

        /**
         * @todo Needs documentation
         */
        void Do(std::function< void() > task) {
            if (std::this_thread::get_id() == _dispatcher.get_id()) {
                task();
            } else {
                std::shared_ptr< TaskWrapper > wrapper = GetTaskWrapper();
                wrapper->hasWaiter = true;
                wrapper->done = false;
                Post(task, wrapper);
                {
                    std::unique_lock< std::mutex > doneLock(wrapper->doneMutex);
                    while (!wrapper->done) {
                        wrapper->doneCondition.wait(doneLock);
                    }
                }
                RecycleTaskWrapper(wrapper);
            }
        }

        // Private methods
    private:
        /**
         * @todo Needs documentation
         */
        std::shared_ptr< TaskWrapper > GetTaskWrapper() {
            std::shared_ptr< TaskWrapper > wrapper;
            if (!_recycledTaskWrappers.Remove(wrapper)) {
                wrapper = std::make_shared< TaskWrapper >();
            }
            return wrapper;
        }

        /**
         * @todo Needs documentation
         */
        void Post(
            std::function< void() > task,
            std::shared_ptr< TaskWrapper > wrapper
        ) {
            wrapper->f = task;
            _tasksToBeDone.Add(wrapper);
            {
                std::lock_guard< std::mutex > dispatchLock(_dispatchMutex);
                _dispatchCondition.notify_one();
            }
        }

        /**
         * @todo Needs documentation
         */
        void RecycleTaskWrapper(std::shared_ptr< TaskWrapper > wrapper) {
            wrapper->f = nullptr;
            _recycledTaskWrappers.Add(wrapper);
        }

        // Private properties
    private:
        /**
         * @todo Needs documentation
         */
        MultiProducerSingleConsumerQueue< std::shared_ptr< TaskWrapper > > _tasksToBeDone;

        /**
         * @todo Needs documentation
         */
        MultiProducerMultiConsumerStack< std::shared_ptr< TaskWrapper > > _recycledTaskWrappers;

        /**
         * @todo Needs documentation
         */
        std::thread _dispatcher;

        /**
         * @todo Needs documentation
         */
        bool _stop = false;

        /**
         * @todo Needs documentation
         */
        std::mutex _dispatchMutex;

        /**
         * @todo Needs documentation
         */
        std::condition_variable _dispatchCondition;
    };

}

#endif /* DATA_STRUCTURES_DISPATCHER_HPP */
