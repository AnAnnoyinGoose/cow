cmake_minimum_required(VERSION 3.23)
project(cow)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra -Werror -Wpedantic -DDEBUG -g -lm")

# All files will be in the source folder
set(SOURCE_FILES src/main.cpp src/server.cpp)


add_executable(cow ${SOURCE_FILES})
