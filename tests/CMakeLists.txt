cmake_minimum_required(VERSION 3.23)
project(test CXX)

# Add source files
find_package(Catch2 3 REQUIRED)

set(SOURCE_FILES ../processor.cpp)
add_executable(tests test_main.cpp ${SOURCE_FILES})

target_link_libraries(tests Catch2::Catch2WithMain)