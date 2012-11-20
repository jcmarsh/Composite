#!/bin/sh

# james test

./cos_loader \
"c0.o, ;llboot.o, ;james.o, :\
\
c0.o-llboot.o;\
james.o-llboot.o\
" ./gen_client_stub
