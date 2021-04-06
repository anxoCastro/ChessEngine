cmake_minimum_required(VERSION 3.16)
project(ChessEngineC C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")
add_executable(ChessEngineC main.c board.c board.h moves.c moves.h bitops.c bitops.h perft.c perft.h)