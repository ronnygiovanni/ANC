cmake_minimum_required(VERSION 3.20)
project(adi_anc_dsp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(anc_test main.cpp)


target_include_directories(anc_test PRIVATE include)