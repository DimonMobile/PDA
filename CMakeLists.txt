cmake_minimum_required(VERSION 3.5)

project(PDA)

set(HEADERS
    Exception/exception.h
    Exception/information_exception.h 
    Exception/not_found_exception.h
    Exception/params_exception.h 
    Exception/source_exception.h
    Transducer/fst.h
    Transducer/preprocessor.h 
    Transducer/tokenizer.h 
    Transducer/expressiontoken.h
    Utils/defaults.h 
    Utils/settings.h
    Transducer/storefst.h 
    Transducer/generator.h 
    Utils/misc.h 
)

SET(SOURCES
    Exception/exception.cpp
    Exception/information_exception.cpp 
    Exception/not_found_exception.cpp
    Exception/params_exception.cpp
    Exception/source_exception.cpp
    Transducer/preprocessor.cpp
    Transducer/tokenizer.cpp 
    Utils/defaults.cpp 
    Utils/settings.cpp 
    main.cpp 
    Transducer/generator.cpp 
    Utils/misc.cpp
    Transducer/expressiontoken.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

set_property(
    TARGET PDA
    PROPERTY CXX_STANDARD 11
    PROPERTY CXX_STANDARD_REQUIRED ON
)

target_include_directories(PDA PRIVATE ${PROJECT_SOURCE_DIR})