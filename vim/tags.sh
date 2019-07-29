#!/bin/sh

find `pwd` -name "*.[ch]" -o -name "*.cpp" > cscope.files

cscope -bkq -i cscope.files

ctags -R -I __THROW -I __attribute_pure__ -I __nonnull -I __attribute__ --languages=c,c++ --c-kinds=+p --c++-kinds=+p --fields=+iaS --extra=+q 

