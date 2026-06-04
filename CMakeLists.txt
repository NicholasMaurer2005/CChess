# minimum version for C++20
cmake_minimum_required(VERSION 3.25)

# project name is CChess. made of C++ source files
project(CChess LANGUAGES CXX)

# process both subprojects (.lib + .exe)
add_subdirectory(CChess)
add_subdirectory(CChessGUI)

