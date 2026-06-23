#! /bin/bash


./buildlibrary-static.sh

gcc -Lbuild/static test.c -lcheddaboards -lcurl -o test.out
./test.out
