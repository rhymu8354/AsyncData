/**
 * @file DispatcherTests.cpp
 *
 * This module contains the unit tests of the
 * AsyncData::Dispatcher class.
 *
 * © 2018 by Richard Walters
 */

#include <AsyncData/Dispatcher.hpp>
#include <functional>
#include <future>
#include <gtest/gtest.h>

TEST(DispatcherTests, Post) {
    // Arrange
    bool doneFlag = false;
    std::promise< void > donePromise;
    const auto doneFuture = donePromise.get_future();
    const auto task = [&]{
        doneFlag = true;
        donePromise.set_value();
    };
    AsyncData::Dispatcher dispatcher;

    // Act
    dispatcher.Post(task);
    const auto doneFutureStatus = doneFuture.wait_for(std::chrono::milliseconds(100));

    // Assert
    EXPECT_EQ(std::future_status::ready, doneFutureStatus);
    EXPECT_TRUE(doneFlag);
}
