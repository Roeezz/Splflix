cmake_minimum_required(VERSION 3.15)

project(Splflix)

set(CMAKE_CXX_STANDARD 11)


add_executable(Splflix src/Main.cpp src/Watchable.cpp src/Session.cpp src/User.cpp src/Action.cpp)