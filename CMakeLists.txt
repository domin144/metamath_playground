cmake_minimum_required( VERSION 2.8 )

project( proof_converter_inference )

set( sources
    src/main.cc
    src/Metamath_database.cc
    src/Metamath_database_read_write.cc
    src/Tokenizer.cc
    src/Symbol.cc
    src/Symbol_visitors.cc
    src/Statement.cc
    src/Symbol_visitors.cc
    src/Named.cc
    src/Scope.cc
    src/verify.cc
    src/Proof_step.cc
    src/Proof_step_visitor.cc
)

set( headers
    src/Metamath_database.h
    src/Metamath_database_read_write.h
    src/Tokenizer.h
    src/Symbol.h
    src/Statement_visitors.h
    src/Statement.h
    src/Statement_visitors.h
    src/Named.h
    src/Scope.h
    src/verify.h
    src/Proof_step.h
    src/Proof_step_visitor.h
)

#include( src )

if( "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" )
    set( CMAKE_CXX_FLAGS "-std=c++11 -pedantic -Wall -Werror" )
elseif( "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" )
    set( CMAKE_CXX_FLAGS "-std=c++11 -pedantic -Wall -Werror" )
elseif( "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel" )
    # using Intel C++
elseif( "${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC" )
    # using Visual Studio C++
endif()

add_executable( convert ${sources} ${headers} )
