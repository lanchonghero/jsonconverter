cmake_minimum_required(VERSION 3.0.0)

project(jsonconverter)

aux_source_directory(. DIR_SRCS)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
include_directories(./)

add_executable(jsonconverter ${DIR_SRCS})
