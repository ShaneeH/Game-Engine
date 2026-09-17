cmake_minimum_required(VERSION 3.16)
project(GameEngine LANGUAGES CXX)

find_package(SDL3 3.2 REQUIRED CONFIG)

add_executable(GameEngine src/main.cpp src/Game.cpp Player.cpp)
target_compile_features(GameEngine PRIVATE cxx_std_20)
target_link_libraries(GameEngine PRIVATE SDL3::SDL3)
