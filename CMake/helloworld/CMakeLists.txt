cmake_minimum_required(VERSION 2.8)
PROJECT(HELLO)
MESSAGE(STATUS "This is a binary dir" ${HELLO_BINARY_DIR})
MESSAGE(STATUS "This is a source dir" ${HELLO_SOURCE_DIR})
ADD_SUBDIRECTORY(src bin)



