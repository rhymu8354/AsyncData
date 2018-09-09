#ifndef ASYNC_DATA_MULTI_PRODUCER_MULTI_CONSUMER_STACK_HPP
#define ASYNC_DATA_MULTI_PRODUCER_MULTI_CONSUMER_STACK_HPP

/**
 * @file MultiProducerMultiConsumerStack.hpp
 *
 * This module declares the AsyncData::MultiProducerMultiConsumerStack
 * class template.
 *
 * © 2015-2018 by Richard Walters
 */

#include <atomic>

namespace AsyncData {

    /**
     * @todo Needs documentation
     */
    template< typename T > class MultiProducerMultiConsumerStack {
        // Types
    public:
        /**
         * @todo Needs documentation
         */
        struct Node {
            T item;
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
        MultiProducerMultiConsumerStack(MultiProducerMultiConsumerStack&&) noexcept = default;
        MultiProducerMultiConsumerStack& operator=(const MultiProducerMultiConsumerStack&) = delete;
        MultiProducerMultiConsumerStack& operator=(MultiProducerMultiConsumerStack&&) noexcept = default;

        // Public Methods
    public:
        MultiProducerMultiConsumerStack()
            : _top(nullptr)
        {
        }

        /**
         * @todo Needs documentation
         */
        void Add(T item) {
            Node* node = new Node;
            node->item = item;
            while (!_top.compare_exchange_weak(node->next, node)) {
            }
        }

        /**
         * @todo Needs documentation
         */
        bool IsEmpty() const {
            return (_top.load() == nullptr);
        }

        /**
         * @todo Needs documentation
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
         * @todo Needs documentation
         */
        std::atomic< Node* > _top;
    };

}

#endif /* ASYNC_DATA_MULTI_PRODUCER_MULTI_CONSUMER_STACK_HPP */
