# CMakeLists.txt for AsyncDataTests
#
# © 2018 by Richard Walters

cmake_minimum_required(VERSION 3.8)
set(This AsyncDataTests)

set(Sources
    src/DispatcherTests.cpp
    src/MultiProducerMultiConsumerStackTests.cpp
    src/MultiProducerSingleConsumerQueueTests.cpp
)

add_executable(${This} ${Sources})
set_target_properties(${This} PROPERTIES
    FOLDER Tests
)

target_include_directories(${This} PRIVATE ../src)

target_link_libraries(${This} PUBLIC
    gtest_main
    AsyncData
)

add_test(
    NAME ${This}
    COMMAND ${This}
)
