# minimum version for C++20
cmake_minimum_required(VERSION 3.25)

# specify C++ version
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# project name is CChess. made of C++ source files
project(CChess LANGUAGES CXX)

# process both subprojects (.lib + .exe)
add_subdirectory(CChess)
add_subdirectory(CChessGUI)

# add option for AVX2 
option(USE_AVX2 "Enable AVX2 optimizations" OFF)