# CMakeLists.txt for AsyncData
#
# © 2018 by Richard Walters

cmake_minimum_required(VERSION 3.8)
set(This AsyncData)

set(Headers
    include/AsyncData/Dispatcher.hpp
    include/AsyncData/MultiProducerMultiConsumerStack.hpp
    include/AsyncData/MultiProducerSingleConsumerQueue.hpp
)

set(Sources
    src/Dispatcher.cpp
)

add_library(${This} STATIC ${Sources} ${Headers})
set_target_properties(${This} PROPERTIES
    FOLDER Libraries
)

target_include_directories(${This} PUBLIC include)

add_subdirectory(test)
