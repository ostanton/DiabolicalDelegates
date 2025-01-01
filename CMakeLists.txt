cmake_minimum_required(VERSION 3.23)
project(ostanton_dd)

add_library(ostanton_dd)
add_library(ostanton::dd ALIAS ostanton_dd)

target_sources(
    ostanton_dd
    PUBLIC
        FILE_SET CXX_MODULES
        FILES
            diabolical_delegates.cpp
)

target_compile_features(ostanton_dd PUBLIC cxx_std_23)
