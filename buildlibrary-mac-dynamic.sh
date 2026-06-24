#!/bin/bash

clang -dynamiclib cheddaboards.c -o libcheddaboards.1.dylib -current_version 1.0 -compatibility_version1.0 -lcurl

mkdir -p build/mac/
mv libcheddaboards.1.dylib build/mac/


