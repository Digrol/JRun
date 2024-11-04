cmake_minimum_required(VERSION 3.15)
project(jrun LANGUAGES C CXX)

set(CMAKE_C_STANDARD 99)

include_directories(./libJRun)

file( GLOB_RECURSE libJRun
	./libJRun/*.cpp
)

add_executable(jrun
    ${libJRun}
    ./jrun/jrun.cpp
)

target_compile_features(jrun PUBLIC cxx_std_17)
