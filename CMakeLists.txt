cmake_minimum_required(VERSION 3.18.2)
project(Edi-TextEditor)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++17 -Wall -Wextra -Wunused-function -lGL -lGLEW -lSDL2 -O3")
file(GLOB_RECURSE SOURCE_FILES "./src/*.cpp")

add_executable(
	edi
	${SOURCE_FILES}
)
