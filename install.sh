#!/bin/sh

INC_DIR=/usr/include
LIB_DIR=/usr/lib

cp -v bin/libcsv.so "${LIB_DIR}"
cp -v src/csv.h "${INC_DIR}"

