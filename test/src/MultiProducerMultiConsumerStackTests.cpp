/**
 * @file MultiProducerMultiConsumerStackTests.cpp
 *
 * This module contains the unit tests of the
 * AsyncData::MultiProducerMultiConsumerStack class.
 *
 * © 2018 by Richard Walters
 */

#include <gtest/gtest.h>
#include <AsyncData/MultiProducerMultiConsumerStack.hpp>

TEST(MultiProducerMultiConsumerStackTests, PushPop) {
    AsyncData::MultiProducerMultiConsumerStack< int > stack;
    EXPECT_TRUE(stack.IsEmpty());
    stack.Add(1);
    EXPECT_FALSE(stack.IsEmpty());
    stack.Add(2);
    EXPECT_FALSE(stack.IsEmpty());
    int first, second;
    stack.Remove(first);
    EXPECT_FALSE(stack.IsEmpty());
    stack.Remove(second);
    EXPECT_TRUE(stack.IsEmpty());
    EXPECT_EQ(2, first);
    EXPECT_EQ(1, second);
}
