#!/bin/sh

find `pwd` -name "*.[ch]" -o -name "*.cpp" > cscope.files

cscope -bkq -i cscope.files

ctags -R
