# src CMakeLists.txt

# Setting boost library variables
# Boost
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost COMPONENTS system regex filesystem)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    add_executable(tcp_server server.cpp)
    target_link_libraries(tcp_server ${Boost_LIBRARIES})
endif()