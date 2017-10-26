cmake_minimum_required(VERSION 3.8)
project(data)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
		logica.c
		logica.h
		main.c
		makefile
		utils.c
		utils.h)

add_executable(data ${SOURCE_FILES})