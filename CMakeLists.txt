cmake_minimum_required(VERSION 3.5)

project(hedg VERSION 1.0 DESCRIPTION "high frequency betfair trading framework")

#include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")


# set(SOURCES  /mnt/c/Users/tosin/linux/cpp_projects/hedg/include/api/messages.h /mnt/c/Users/tosin/linux/cpp_projects/hedg/include/api/params.h
#  /mnt/c/Users/tosin/linux/cpp_projects/hedg/include/client/authclient.h /mnt/c/Users/tosin/linux/cpp_projects/hedg/include/client/client.h
#  /mnt/c/Users/tosin/linux/cpp_projects/hedg/include/client/restclient.h /mnt/c/Users/tosin/linux/cpp_projects/hedg/include/client/streamclient.h 
#  /mnt/c/Users/tosin/linux/cpp_projects/hedg/include/config/config.h /mnt/c/Users/tosin/linux/cpp_projects/hedg/include/fmt/core.h
#   /mnt/c/Users/tosin/linux/cpp_projects/hedg/include/fmt/format-inl.h /mnt/c/Users/tosin/linux/cpp_projects/hedg/include/fmt/format.h 
#   /mnt/c/Users/tosin/linux/cpp_projects/hedg/include/market/bid.h 
#  /mnt/c/Users/tosin/linux/cpp_projects/hedg/include/market/marketprojection.h /mnt/c/Users/tosin/linux/cpp_projects/hedg/include/market/order.h
#   /mnt/c/Users/tosin/linux/cpp_projects/hedg/include/market/structures/orderbook.h 
#  /mnt/c/Users/tosin/linux/cpp_projects/hedg/include/market/structures/tradedbook.h /mnt/c/Users/tosin/linux/cpp_projects/hedg/include/simdjson/simdjson.h
#   /mnt/c/Users/tosin/linux/cpp_projects/hedg/include/timing/timing.h 
#  /mnt/c/Users/tosin/linux/cpp_projects/hedg/include/utility/ctpl.h /mnt/c/Users/tosin/linux/cpp_projects/hedg/include/utility/queue.h 
#  /mnt/c/Users/tosin/linux/cpp_projects/hedg/include/utility/utils.h 
#  /mnt/c/Users/tosin/linux/cpp_projects/hedg/include/workers/marketgenerator.h /mnt/c/Users/tosin/linux/cpp_projects/hedg/include/workers/markethandler.h 
#  /mnt/c/Users/tosin/linux/cpp_projects/hedg/include/workers/parser.h /mnt/c/Users/tosin/linux/cpp_projects/hedg/include/workers/publisher.h
#  /mnt/c/Users/tosin/linux/cpp_projects/hedg/include/framework/basestrategy.h /mnt/c/Users/tosin/linux/cpp_projects/hedg/include/framework/framework.h
#  )

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

# add_executable(hedg ${SOURCES}
#     src/api/messages.cpp
#     src/fmt/format.cpp
#     src/utility/utils.cpp
#     src/workers/publisher.cpp
#     src/api/params.cpp     
#     src/simdjson/simdjson.cpp  
#     src/workers/marketgenerator.cpp
#     src/config/config.cpp  
#     src/timing/timing.cpp      
#     src/workers/parser.cpp
#     examples/vwapstrategy.cpp
# )


# install(TARGETS hedg
#     LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
#     PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})



# configure_file(hedg.pc.in hedg.pc @ONLY)

# install(FILES ${CMAKE_BINARY_DIR}/hedg.pc DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig)


set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 
find_package(Boost 1.71.0 COMPONENTS system) 

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





# set_target_properties(hedg PROPERTIES VERSION ${PROJECT_VERSION})
# set_target_properties(hedg PROPERTIES PUBLIC_HEADER include/framework/framework.h)

# target_include_directories(hedg PUBLIC include)
# target_include_directories(hedg PRIVATE src)

# include_directories(${CMAKE_SOURCE_DIR}/libs/)




#add_subdirectory(examples)

# target_link_libraries(vwap hedg)

# add_executable(vwap examples/vwapstrategy.cpp)


