cmake_minimum_required(VERSION 3.25)
project(Progect_Labirint)

set(CMAKE_CXX_STANDARD 17)

add_executable(Progect_Labirint main.cpp Labirint.h Labirint.cpp Game.cpp Character.h Character.cpp Robot.h Robot.cpp Minotaur.cpp)

find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)
target_link_libraries(Progect_Labirint sfml-graphics sfml-window sfml-system)

