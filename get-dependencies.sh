#!/bin/sh

SDL3_LINK="https://github.com/libsdl-org/SDL/releases/download/preview-3.1.6/SDL3-3.1.6.tar.gz"
SDL3_ARCHIVE="SDL3-3.1.6.tar.gz"
SDL3_DIR="SDL3-3.1.6"

XZ_UTILS_LINK="https://github.com/tukaani-project/xz/releases/download/v5.6.3/xz-5.6.3.tar.xz"
XZ_UTILS_ARCHIVE="xz-5.6.3.tar.xz"
XZ_UTILS_DIR="xz-5.6.3"

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

if [ $CC = "x86_64-w64-mingw32-gcc" ]; then
	if ! [ -f "$SDL3_DIR/build/SDL3.dll" ]; then
		cd "$ORIGIN_DIR/$SDL3_DIR" || exit 1
		cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=build-scripts/cmake-toolchain-mingw64-x86_64.cmake -DCMAKE_BUILD_TYPE=Release -DSDL_AUDIO=OFF -DSDL_HAPTIC=OFF -DSDL_JOYSTICK=OFF -DSDL_GPU=OFF -DSDL_POWER=OFF -DSDL_CAMERA=OFF -DSDL_SENSOR=OFF -DSDL_HIDAPI=OFF
		cmake --build build -j "$(nproc)"
		cd "$ORIGIN_DIR" || exit 1
	fi
else
	if ! [ -f "$SDL3_DIR/build/libSDL3.so.0" ]; then
		cd "$ORIGIN_DIR/$SDL3_DIR" || exit 1
		cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DSDL_AUDIO=OFF -DSDL_HAPTIC=OFF -DSDL_JOYSTICK=OFF -DSDL_GPU=OFF -DSDL_POWER=OFF -DSDL_CAMERA=OFF -DSDL_SENSOR=OFF -DSDL_HIDAPI=OFF
		cmake --build build -j "$(nproc)"
		cd "$ORIGIN_DIR" || exit 1
	fi
fi

if ! [ -d "build/" ]; then
	mkdir build
fi

if [ $CC = "x86_64-w64-mingw32-gcc" ]; then
	cp "$SDL3_DIR/build/SDL3.dll" build/
else
	cp "$SDL3_DIR/build/libSDL3.so" "$SDL3_DIR/build/libSDL3.so.0" build/
fi


if ! [ -f "$XZ_UTILS_ARCHIVE" ]; then
	wget "$XZ_UTILS_LINK" -O "$XZ_UTILS_ARCHIVE"
fi

if ! [ -d "$XZ_UTILS_DIR" ]; then
	tar -xavf "$XZ_UTILS_ARCHIVE"
fi

if ! [ -f "$XZ_UTILS_DIR/src/liblzma/.libs/liblzma.a" ]; then
	cd "$ORIGIN_DIR/$XZ_UTILS_DIR" || exit 1
	if [ $CC = "x86_64-w64-mingw32-gcc" ]; then
		cp "/usr/share/licenses/mingw-w64-headers/COPYING.MinGW-w64-runtime.txt" "./windows/"
		bash windows/build.bash
	else
		./configure
		make -j12
		cd "$ORIGIN_DIR" || exit 1
	fi
	cd "$ORIGIN_DIR" || exit 1
fi

export INCLUDES="$INCLUDES -I$SDL3_DIR/include -I$XZ_UTILS_DIR/src/liblzma/api"
export LIBS="$LIBS -Wl,-rpath=./ -Wl,-rpath=build/ -L./build/ -lSDL3 $XZ_UTILS_DIR/src/liblzma/.libs/liblzma.a"

