#!/bin/sh

SDL3_LINK="https://github.com/libsdl-org/SDL/releases/download/preview-3.1.6/SDL3-3.1.6.tar.gz"
SDL3_ARCHIVE="SDL3-3.1.6.tar.gz"
SDL3_DIR="SDL3-3.1.6"

if [ -z "$ORIGIN_DIR" ]; then
	ORIGIN_DIR="$(readlink -f "$(dirname "$0")")"
	cd "$ORIGIN_DIR" || exit 1
fi

if ! [ -f "$SDL3_ARCHIVE" ]; then
	wget "$SDL3_LINK" -O "$SDL3_ARCHIVE"
fi

if ! [ -d "$SDL3_DIR" ]; then
	tar -xavf "$SDL3_ARCHIVE"
fi

if ! [ -f "$SDL3_DIR/build/libSDL3.so.0" ]; then
	cd "$ORIGIN_DIR/$SDL3_DIR" || exit 1
	cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DSDL_AUDIO=OFF -DSDL_HAPTIC=OFF -DSDL_JOYSTICK=OFF -DSDL_GPU=OFF -DSDL_POWER=OFF -DSDL_CAMERA=OFF -DSDL_SENSOR=OFF -DSDL_HIDAPI=OFF
	cmake --build build -j "$(nproc)"
	cd "$ORIGIN_DIR" || exit 1
fi

if ! [ -d "build/" ]; then
	mkdir build
fi

cp "$SDL3_DIR/build/libSDL3.so" "$SDL3_DIR/build/libSDL3.so.0" build/

export INCLUDES="$INCLUDES -I$SDL3_DIR/include"
export LIBS="$LIBS -Wl,-rpath=./ -Wl,-rpath=build/ -L./build/ -lSDL3"

