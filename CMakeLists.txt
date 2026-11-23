cmake_minimum_required(VERSION 3.20)
project(battlecity CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(raylib REQUIRED)

add_executable(battlecity
    src/main.cpp
    src/Game.cpp
    src/Tank.cpp
    src/Map.cpp
    src/Bullet.cpp
    src/Menu.cpp
)

target_link_libraries(battlecity PRIVATE raylib)
