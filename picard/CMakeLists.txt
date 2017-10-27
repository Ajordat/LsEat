cmake_minimum_required(VERSION 3.8)
project(picard)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
		config.dat
		logica.c
		logica.h
		main.c
		makefile
		utils.c
		utils.h network.c network.h)

add_executable(picard ${SOURCE_FILES})