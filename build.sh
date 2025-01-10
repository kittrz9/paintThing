#!/bin/sh

set -xe

CC=clang
CFLAGS="-Wall -Wextra -Wpedantic -std=c99 -g -D_XOPEN_SOURCE=700"
LIBS="-lm -llzma"
INCLUDES=""

CFILES="$(find src/ -name "*.c")"

SRC_DIRS="$(find src/ -mindepth 1 -type d)"

rm -rf build/ obj/
mkdir build/ obj/

# affects $LIBS and $INCLUDES
. ./get-dependencies.sh

mkdir -p src/generated
./imageToC.py font.bmp

for d in $SRC_DIRS; do
	OBJDIR="$(echo $d | sed -e "s/src/obj/")"
	mkdir -p $OBJDIR
done

for f in $CFILES; do
	OBJNAME="$(echo $f | sed -e "s/src/obj/;s/\.c/\.o/")"
	$CC $INCLUDES $CFLAGS -c $f -o $OBJNAME
	OBJS="$OBJS $OBJNAME"
done

$CC $OBJS $LIBS -o build/paint
