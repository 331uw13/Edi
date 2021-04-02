cmake_minimum_required(VERSION 3.18.2)
project(Edi-TextEditor C)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -lX11")
file(GLOB_RECURSE SOURCE_FILES "./src/*.c")

add_executable(
	edi
	${SOURCE_FILES}
)


