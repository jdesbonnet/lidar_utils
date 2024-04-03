#!/bin/bash
CFLAGS=""

gcc -c lvx.c
gcc $CFLAGS -o lvx_read main.c
gcc $CFLAGS -o lvx_split split.c
 

