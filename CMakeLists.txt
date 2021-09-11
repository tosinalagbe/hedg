cmake_minimum_required(VERSION 3.5)

project(hedg VERSION 1.0 DESCRIPTION "high frequency betfair trading framework")


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")


set(SOURCES
    src/api/messages.cpp
    src/api/params.cpp 
    src/config/config.cpp
    src/fmt/format.cpp
    src/workers/publisher.cpp
    src/market/structures/tradedbook.cpp
    src/market/order.cpp
    src/simdjson/simdjson.cpp  
    src/workers/marketgenerator.cpp
    src/timing/timing.cpp   
    src/utility/utils.cpp 
    src/workers/marketgenerator.cpp  
    src/workers/parser.cpp
    src/workers/publisher.cpp
    examples/vwapstrategy.cpp
)

add_executable(hedg ${SOURCES})

target_include_directories(hedg PRIVATE ${PROJECT_SOURCE_DIR}/include)


set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 
find_package(Boost 1.67.0 COMPONENTS system) 

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS}) 
    target_link_libraries(hedg ${Boost_LIBRARIES})
endif()

find_package(OpenSSL MODULE REQUIRED)
target_link_libraries(hedg OpenSSL::SSL)
list(APPEND CMAKE_REQUIRED_LIBRARIES ${OPENSSL_LIBRARIES})
list(APPEND CMAKE_REQUIRED_INCLUDES ${OPENSSL_INCLUDE_DIR})
list(REMOVE_ITEM CMAKE_REQUIRED_LIBRARIES ${OPENSSL_LIBRARIES})
list(REMOVE_ITEM CMAKE_REQUIRED_INCLUDES ${OPENSSL_INCLUDE_DIR})




