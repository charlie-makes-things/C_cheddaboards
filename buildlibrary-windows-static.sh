#! /bin/bash

mkdir -p build/win

libpath="/home/charlie/Documents/projects/include/curl-win64-mingw/include"

x86_64-w64-mingw32-gcc -I$libpath -c cheddaboards.c -o build/cheddaboards.o
x86_64-w64-mingw32-ar rcs -o build/libcheddaboards.a build/cheddaboards.o
mv build/libcheddaboards.a build/win/libcheddaboards.a
rm build/cheddaboards.o