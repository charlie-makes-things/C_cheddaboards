#! /bin/bash


./buildlibrary-dynamic.sh

export LD_LIBRARY_PATH=$PWD/build/dylib:$LD_LIBRARY_PATH

gcc -L$PWD/build/dylib test.c -lcheddaboards -lcurl -o test.out
./test.out
