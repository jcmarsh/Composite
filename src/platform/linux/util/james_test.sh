#!/bin/sh

# james test

./cos_loader \
"c0.o, ;llboot.o, ;james.o, ;print.o, :\
\
c0.o-llboot.o;\
james.o-print.o\
" ./gen_client_stub
