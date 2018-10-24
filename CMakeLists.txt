cmake_minimum_required(VERSION 2.8)

project(PDA)
include_directories(${PROJECT_NAME})
add_executable(${PROJECT_NAME} main.cpp Utils/settings.cpp Exception/exception.cpp Exception/information_exception.cpp Transducer/fst.h)
