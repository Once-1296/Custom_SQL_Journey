cmake_minimum_required(VERSION 3.16)

project(bsql VERSION 1.0.0 LANGUAGES CXX)

add_executable(bsql main.cpp)
target_compile_features(bsql PRIVATE cxx_std_20)
target_include_directories(bsql PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")
set_target_properties(bsql PROPERTIES CXX_EXTENSIONS OFF)

install(TARGETS bsql RUNTIME DESTINATION bin)
