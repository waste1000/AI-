cmake_minimum_required(VERSION 3.10)
project(aic-kali-sim)
add_executable(kernelsim src/main.cpp)
set_target_properties(kernelsim PROPERTIES CXX_STANDARD 17)