#! /bin/bash

gcc -c -fPIC cheddaboards.c -o build/cheddaboards.o
gcc -shared -o build/dylib/libcheddaboards.so build/cheddaboards.o
rm build/cheddaboards.o