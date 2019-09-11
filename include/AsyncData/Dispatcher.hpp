#pragma once

/**
 * @file Dispatcher.hpp
 *
 * This module declares the AsyncData::Dispatcher class.
 *
 * © 2015-2019 by Richard Walters
 */

#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>

namespace AsyncData {

    /**
     * This class operates a worker thread which executes jobs.  A job consists
     * of a function to be called by the worker thread and a mechanism through
     * which another thread can poll or wait for the function call to be
     * completed and obtain the return value, if any.
     */
    class Dispatcher {
        // Types
    public:
        /**
         * This is an entry in the dispatcher's queue, represing some
         * kind of work to do, and the objects used to coordinate
         * with the client of the work.
         */
        struct TaskWrapper {
            /**
             * This is the function to call in order to perform the requested
             * work.
             */
            std::function< void() > f;

            /**
             * This is set to true if the client is waiting for the work
             * to be completed.
             */
            bool hasWaiter = false;

            /**
             * If the client is waiting for the work to be completed, this is
             * used to coordinate waking up the client.
             */
            std::mutex doneMutex;

            /**
             * If the client is waiting for the work to be completed, this is
             * notified in order to wake up the client.
             */
            std::condition_variable doneCondition;

            /**
             * If the client is waiting for the work to be completed, this is
             * set once the work is completed.
             */
            bool done = false;
        };

        // Lifecycle Management
    public:
        ~Dispatcher() noexcept;
        Dispatcher(const Dispatcher&) = delete;
        Dispatcher(Dispatcher&&) noexcept;
        Dispatcher& operator=(const Dispatcher&) = delete;
        Dispatcher& operator=(Dispatcher&&) noexcept;

        // Public Methods
    public:
        /**
         * This is the default constructor.
         */
        Dispatcher();

        /**
         * Immediately cancel and drop all queued work that has not yet been
         * started, wait for any work in progress to be completed, and
         * join the worker thread.
         */
        void Stop();

        /**
         * Add a function to the queue of the dispatcher, to be called
         * from the dispatcher's worker thread, once all previously
         * queued tasks have been completed.
         */
        void Post(std::function< void() > task);

        // Private properties
    private:
        /**
         * This is the type of structure that contains the private
         * properties of the instance.  It is defined in the implementation
         * and declared here to ensure that it is scoped inside the class.
         */
        struct Impl;

        /**
         * This contains the private properties of the instance.
         */
        std::unique_ptr< Impl > impl_;
    };

}
