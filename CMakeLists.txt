cmake_minimum_required(VERSION 3.16)
project(GameEngine LANGUAGES CXX)

option(GAMEENGINE_BUILD_TESTS "Build the headless engine tests" ON)

find_package(SDL3 3.2 REQUIRED CONFIG)

add_library(Engine STATIC
    src/engine/Systems.cpp
)
target_include_directories(Engine PUBLIC src)
target_compile_features(Engine PUBLIC cxx_std_20)
target_link_libraries(Engine PUBLIC SDL3::SDL3)

add_executable(GameEngine
    src/main.cpp
    src/game/Game.cpp
    src/game/GameSystems.cpp
)
target_link_libraries(GameEngine PRIVATE Engine)

if(MSVC)
    target_compile_options(Engine PRIVATE /W4)
    target_compile_options(GameEngine PRIVATE /W4)
else()
    target_compile_options(Engine PRIVATE -Wall -Wextra)
    target_compile_options(GameEngine PRIVATE -Wall -Wextra)
endif()

if(GAMEENGINE_BUILD_TESTS)
    enable_testing()
    # The registry is header only and doesn't need SDL.
    add_executable(RegistryTests tests/RegistryTests.cpp)
    target_compile_features(RegistryTests PRIVATE cxx_std_20)
    if(NOT MSVC)
        target_compile_options(RegistryTests PRIVATE -Wall -Wextra)
    endif()
    add_test(NAME RegistryTests COMMAND RegistryTests)
endif()
