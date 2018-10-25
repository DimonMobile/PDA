cmake_minimum_required(VERSION 2.8)

project(PDA)
include_directories(${PROJECT_NAME})

set(Utils Utils/settings.cpp)
set(Exception Exception/exception.cpp Exception/information_exception.cpp Exception/params_exception)
set(Transducer Transducer/fst.h Transducer/preprocessor.cpp)
add_executable(${PROJECT_NAME} main.cpp ${Utils} ${Exception} ${Transducer})
