/**
 * @file MultiProducerSingleConsumerQueueTests.cpp
 *
 * This module contains the unit tests of the
 * AsyncData::MultiProducerSingleConsumerQueue class.
 *
 * © 2018 by Richard Walters
 */

#include <gtest/gtest.h>
#include <AsyncData/MultiProducerSingleConsumerQueue.hpp>

TEST(MultiProducerSingleConsumerQueueTests, PushPop) {
    AsyncData::MultiProducerSingleConsumerQueue< int > queue;
    EXPECT_TRUE(queue.IsEmpty());
    queue.Add(1);
    EXPECT_FALSE(queue.IsEmpty());
    queue.Add(2);
    EXPECT_FALSE(queue.IsEmpty());
    int first, second;
    first = queue.Remove();
    EXPECT_FALSE(queue.IsEmpty());
    second = queue.Remove();
    EXPECT_TRUE(queue.IsEmpty());
    EXPECT_EQ(1, first);
    EXPECT_EQ(2, second);
}
