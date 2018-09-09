#ifndef ASYNC_DATA_MULTI_PRODUCER_SINGLE_CONSUMER_QUEUE_HPP
#define ASYNC_DATA_MULTI_PRODUCER_SINGLE_CONSUMER_QUEUE_HPP

/**
 * @file MultiProducerSingleConsumerQueue.hpp
 *
 * This module declares the AsyncData::MultiProducerSingleConsumerQueue
 * class template.
 *
 * © 2015-2018 by Richard Walters
 */

#include <atomic>

namespace AsyncData {

    /**
     * This is a template for a lockless multi-producer single-consumer queue.
     *
     * It is based on the algorithm described here:
     * http://www.1024cores.net/home/lock-free-algorithms/queues/non-intrusive-mpsc-node-based-queue
     */
    template< typename T > class MultiProducerSingleConsumerQueue {
        // Types
    public:
        /**
         * This structure holds a single item in the queue, and
         * a reference to the next item in the queue.
         */
        struct Node {
            /**
             * This is the item held by the node.
             */
            T item;

            /**
             * This is a reference to the next item in the queue.
             * If nullptr, the node is the last in the queue
             * or has not yet been connected to nodes currently
             * still being added to the queue.
             */
            struct Node* next = nullptr;
        };

        // Lifecycle Management
    public:
        ~MultiProducerSingleConsumerQueue() noexcept {
            while (_head != nullptr) {
                Node* node = _head;
                _head = node->next;
                delete node;
            }
        }
        MultiProducerSingleConsumerQueue(const MultiProducerSingleConsumerQueue&) = delete;
        MultiProducerSingleConsumerQueue(MultiProducerSingleConsumerQueue&&) = default;
        MultiProducerSingleConsumerQueue& operator=(const MultiProducerSingleConsumerQueue&) = delete;
        MultiProducerSingleConsumerQueue& operator=(MultiProducerSingleConsumerQueue&&) = default;

        // Public Methods
    public:
        /**
         * This is the object constructor.
         */
        MultiProducerSingleConsumerQueue() {
            _head = _tail = new Node();
        }

        /**
         * This method adds the given item to the back of the queue.
         * It is safe for multiple threads to call this method
         * concurrently.
         *
         * @param[in] item
         *     This is the item to add to the back of the queue.
         */
        void Add(T item) {
            Node* node = new Node;
            node->item = item;
            _tail.exchange(node)->next = node;
        }

        /**
         * This method determines whether or not the queue is empty.
         *
         * @return
         *     A flag indicating whether or not the queue is empty
         *     is returned.
         */
        bool IsEmpty() const {
            return (_head->next == nullptr);
        }

        /**
         * This method removes and returns the item at the front of the queue.
         * Only one thread at a time should call this method.
         */
        T Remove() {
            Node* node = _head;
            _head = node->next;
            T item = _head->item;
            _head->item = T();
            delete node;
            return item;
        }

        // Private Properties
    private:
        /**
         * This points to a sentinel node with an empty value, whose next
         * pointer refers to the current front of the queue.
         */
        Node* _head;

        /**
         * This points to the node at the back of the queue, or the sentinel
         * node if the queue is empty.
         */
        std::atomic< Node* > _tail;
    };

}

#endif /* ASYNC_DATA_MULTI_PRODUCER_SINGLE_CONSUMER_QUEUE_HPP */
