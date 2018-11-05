cmake_minimum_required(VERSION 2.8)

project(PDA)
include_directories(${PROJECT_NAME})

set(Utils Utils/settings.cpp Utils/defaults.cpp)
set(Exception Exception/exception.cpp Exception/information_exception.cpp Exception/params_exception Exception/source_exception.cpp Exception/not_found_exception.cpp)
set(Transducer Transducer/fst.h Transducer/preprocessor.cpp Transducer/tokenizer.cpp)
add_executable(${PROJECT_NAME} main.cpp ${Utils} ${Exception} ${Transducer})
