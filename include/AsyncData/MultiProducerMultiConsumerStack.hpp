#pragma once

/**
 * @file MultiProducerMultiConsumerStack.hpp
 *
 * This module declares the AsyncData::MultiProducerMultiConsumerStack
 * class template.
 *
 * © 2015-2019 by Richard Walters
 */

#include <atomic>

namespace AsyncData {

    /**
     * This implements a stack of arbitrary items, where it's safe to push onto
     * and pull items off of the stack from different threads without the use
     * of locking mechanisms such as mutexes.
     */
    template< typename T > class MultiProducerMultiConsumerStack {
        // Types
    public:
        /**
         * This holds one item on the stack.
         */
        struct Node {
            /**
             * This provides storage for the item.
             */
            T item;

            /**
             * This points to the next item on the stack.
             */
            struct Node* next = nullptr;
        };

        // Lifecycle Management
    public:
        ~MultiProducerMultiConsumerStack() noexcept {
            while (!IsEmpty()) {
                T item;
                (void)Remove(item);
            }
        }
        MultiProducerMultiConsumerStack(const MultiProducerMultiConsumerStack&) = delete;
        MultiProducerMultiConsumerStack(MultiProducerMultiConsumerStack&&) = default;
        MultiProducerMultiConsumerStack& operator=(const MultiProducerMultiConsumerStack&) = delete;
        MultiProducerMultiConsumerStack& operator=(MultiProducerMultiConsumerStack&&) = default;

        // Public Methods
    public:
        MultiProducerMultiConsumerStack()
            : _top(nullptr)
        {
        }

        /**
         * Push the given item onto the stack.
         *
         * @param[in] item
         *     This is the item to push onto the stack.
         */
        void Add(T item) {
            Node* node = new Node;
            node->item = item;
            while (!_top.compare_exchange_weak(node->next, node)) {
            }
        }

        /**
         * Return an indication of whether or not the stack is empty.
         *
         * @return
         *     An indication of whether or not the stack is empty is returned.
         */
        bool IsEmpty() const {
            return (_top.load() == nullptr);
        }

        /**
         * Pop the item on the top of the stack.
         *
         * @param[out] item
         *     This is where to store the item popped off the stack.
         *
         * @return
         *     An indication of whether or not an item was popped off
         *     the stack is returned.  This will be false if the stack
         *     was empty before the call, or if another thread beats us
         *     to it.
         */
        bool Remove(T& item) {
            Node* node = _top.load();
            if (node == nullptr) {
                return false;
            }
            while (!_top.compare_exchange_weak(node, node->next)) {
                if (node == nullptr) {
                    return false;
                }
            }
            item = node->item;
            delete node;
            return true;
        }

        // Private Properties
    private:
        /**
         * This points to the top of the stack.
         */
        std::atomic< Node* > _top;
    };

}
