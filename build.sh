#!/bin/sh

set -xe

CC=clang
CFLAGS="-Wall -Wextra -Wpedantic -std=c99 -g -D_XOPEN_SOURCE=700"
LIBS="-lm"
INCLUDES=""

CFILES="$(find src/ -name "*.c")"

rm -rf build/ obj/
mkdir build/ obj/

# affects $LIBS and $INCLUDES
. ./get-dependencies.sh

for f in $CFILES; do
	OBJNAME="$(echo $f | sed -e "s/src/obj/;s/\.c/\.o/")"
	$CC $INCLUDES $CFLAGS -c $f -o $OBJNAME
	OBJS="$OBJS $OBJNAME"
done

$CC $OBJS $LIBS -o build/paint
