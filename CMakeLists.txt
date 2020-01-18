cmake_minimum_required(VERSION 3.16)
project(SNESGAPE VERSION 1.0.0)

add_library(
	gape SHARED
	gape.h
	gape.c
	)

add_executable(snesgape main.c)

target_link_libraries(snesgape PRIVATE gape)

