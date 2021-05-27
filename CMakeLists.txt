cmake_minimum_required(VERSION 3.0.0)
project(PlayBall-Server VERSION 0.1.0)

include(CTest)
enable_testing()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

set(CMAKE_INCLUDE_CURRENT_DIR  ON)

SET(CMAKE_CXX_STANDARD 17)
SET(SDL2_DIR D:\\Sources\\SDL2_Env\\SDL_2.0.14)
SET(SDL2_NET D:\\Sources\\SDL2_Env\\SDL2_net-2.0.1)

include_directories(include)
include_directories(${SDL2_DIR}/include)
include_directories(${SDL2_NET}/include)
link_directories(${SDL2_DIR}/bin)
link_directories(${SDL2_NET}/bin)


set(SOURCE_FILES main.cpp 
                 include/ThreadPool.h
                 include/NetServer.h
                 util/ThreadPool.hpp
                 util/ThreadPool.cpp
                 NetServer/NetServer.cpp)

add_executable(PlayBall-Server ${SOURCE_FILES})
target_link_libraries(PlayBall-Server mingw32 SDL2main SDL2 SDL2_net)