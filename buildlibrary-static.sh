#! /bin/bash

mkdir -p build/static

gcc -c cheddaboards.c -o build/cheddaboards.o
ar rcs -o build/libcheddaboards.a build/cheddaboards.o
mv build/libcheddaboards.a build/static/libcheddaboards.a
rm build/cheddaboards.o