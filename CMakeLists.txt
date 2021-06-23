# github address https://github.com/CRAFTSTARCN/PlayBall-Server
#By Ag2S(Yinchen Liu)

cmake_minimum_required(VERSION 3.0.0)
project(PlayBall-Server VERSION 0.1.0)

include(CTest)
enable_testing()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

set(CMAKE_INCLUDE_CURRENT_DIR  ON)

SET(CMAKE_CXX_STANDARD 17)
SET(SDL2_DIR D:\\Sources\\SDL2_Env\\SDL_2.0.14) # switch to your sdl enviroment dir
SET(SDL2_NET D:\\Sources\\SDL2_Env\\SDL2_net-2.0.1) # switch to your sdl_net enviroment dir

include_directories(include)
include_directories(${SDL2_DIR}/include)
include_directories(${SDL2_NET}/include)
link_directories(${SDL2_DIR}/bin)
link_directories(${SDL2_NET}/bin)


set(SOURCE_FILES main.cpp 
                 include/ThreadPool.h
                 include/ExclusiveSharedLock.h
                 include/ServiceInfo.h
                 include/StrTool.h
                 include/UserAgent.h
                 include/AbstractMessage.h
                 include/GTPMessage.h
                 include/DataObject.h
                 include/ServerListenerInterface.h
                 include/UserListenerInterface.h
                 include/NetServer.h
                 include/MessageParser.h
                 include/MessageListenerInterface.h
                 include/MessageParseLayer.h
                 include/GTPParser.h
                 include/Room.h
                 include/Player.h
                 include/Game.h
                 include/ServerListener.h
                 include/UserListener.h
                 include/MessageListener.h
                 include/ACKMessage.h

                 util/ThreadPool.hpp
                 util/ThreadPool.cpp
                 util/ExclusiveSharedLock.cpp
                 util/DataObject.cpp
                 util/CallnackListener.cpp

                 NetServer/StrTool.cpp
                 NetServer/AbstractMessage.cpp
                 NetServer/UserAgent.cpp
                 NetServer/ServerSideListenerInterfaces.cpp
                 NetServer/MessageListenerInterface.cpp
                 NetServer/NetServer.cpp
                 NetServer/Messages/GTPMessage.cpp
                 NetServer/Messages/ACKMessage.cpp
                 NetServer/MessageParseLayer.cpp
                 NetServer/Parsers/GTPParser.cpp
                 NetServer/ServerListener.cpp
                 NetServer/MessageParser.cpp
                 
                 Game/Player.cpp
                 Game/Room.cpp
                 Game/Game.cpp
                 Game/UserListener.cpp
                 Game/MessageListener.cpp)

add_executable(PlayBall-Server ${SOURCE_FILES})
target_link_libraries(PlayBall-Server mingw32 SDL2main SDL2 SDL2_net)