cmake_minimum_required(VERSION 3.28.1)

project(DiabolicalDelegates LANGUAGES CXX)

add_library(dd INTERFACE)

target_include_directories(dd INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_features(dd INTERFACE cxx_std_17)
