#! /bin/bash

./buildlibrary-static.sh
gcc test.c -Lbuild/static/ -lcheddaboards -lcurl -o test
./test
