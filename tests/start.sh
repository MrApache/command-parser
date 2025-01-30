#!/bin/bash

cd ..
./build.sh
cd tests
./build.sh
export LD_LIBRARY_PATH=.:$LD_LIBRARY_PATH
./main "command $ \"Hello, world@#!\""
