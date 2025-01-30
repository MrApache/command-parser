#!/bin/bash
#mv ../build/libcommand_parser.so .
gcc main.c -L. -lcommand_parser -o main
