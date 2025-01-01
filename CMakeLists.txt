cmake_minimum_required(VERSION 3.23)
project(diabolical_delegates)

add_library(diabolical_delegates)

target_sources(
    diabolical_delegates
    PUBLIC
        FILE_SET CXX_MODULES
        FILES
            diabolical_delegates.cpp
)

target_compile_features(diabolical_delegates PUBLIC cxx_std_23)
