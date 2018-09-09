#ifndef ASYNC_DATA_DISPATCHER_HPP
#define ASYNC_DATA_DISPATCHER_HPP

/**
 * @file Dispatcher.hpp
 *
 * This module declares the AsyncData::Dispatcher class.
 *
 * © 2015-2018 by Richard Walters
 */

#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>

namespace AsyncData {

    /**
     * @todo Needs documentation
     */
    class Dispatcher {
        // Types
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
         * @todo Needs documentation
         */
        void Stop();

        /**
         * @todo Needs documentation
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

#endif /* ASYNC_DATA_DISPATCHER_HPP */
