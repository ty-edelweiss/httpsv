cmake_minimum_required(VERSION 3.10)

configure_file(config.h.in config.h)

add_library(app STATIC cli.cpp http.cpp)

add_executable(httpsv main.cpp)

target_link_libraries(httpsv PUBLIC app)
target_include_directories(httpsv PUBLIC
                           "${PROJECT_BINARY_DIR}"
                           "${PROJECT_CURRENT_SOURCE_DIR}")