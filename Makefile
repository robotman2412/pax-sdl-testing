
# SPDX-License-Identidier: MIT

MAKEFLAGS += --silent --no-print-directory

.PHONY: all build clean run

all: build run

build:
	mkdir -p build
	cmake -B build
	cmake --build build

clean:
	rm -rf build

run: build
	./build/pax-sdl

gdb: build
	gdb ./build/pax-sdl
