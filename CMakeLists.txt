cmake_minimum_required(VERSION 3.10)

project(httpsv
        VERSION "0.0.1"
        DESCRIPTION "The simple http server")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(src)