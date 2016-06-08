#ifndef DATA_STRUCTURES_MULTI_PRODUCER_MULTI_CONSUMER_STACK_HPP
#define DATA_STRUCTURES_MULTI_PRODUCER_MULTI_CONSUMER_STACK_HPP

/**
 * @file MultiProducerMultiConsumerStack.hpp
 *
 * This module declares the DataStructures::MultiProducerMultiConsumerStack
 * class template.
 *
 * Copyright (c) 2015-2016 by Richard Walters
 */

#include <atomic>

namespace DataStructures {

    /**
     * @todo Needs documentation
     */
    template< typename T > class MultiProducerMultiConsumerStack {
        // Custom types
    public:
        /**
         * @todo Needs documentation
         */
        struct Node {
            T item;
            struct Node* next = nullptr;
        };

        // Public methods
    public:
        MultiProducerMultiConsumerStack()
            : _top(nullptr)
        {
        }

        /**
         * @todo Needs documentation
         */
        ~MultiProducerMultiConsumerStack() {
            while (!IsEmpty()) {
                T item;
                (void)Remove(item);
            }
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

        // Private properties
    private:
        /**
         * @todo Needs documentation
         */
        std::atomic< Node* > _top;
    };

}

#endif /* DATA_STRUCTURES_MULTI_PRODUCER_MULTI_CONSUMER_STACK_HPP */
