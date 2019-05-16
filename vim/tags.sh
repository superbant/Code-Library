#!/bin/sh

find `pwd` -name "*.[ch]" -o -name "*.cpp" > cscope.files

cscope -bkq -i cscope.files

ctags -R -I __THROW -I __attribute_pure__ -I __nonnull -I __attribute__ --file-scope=yes
--languages=c,c++ --links=yes --c++-kinds=+p --fields=+ialS --extra=+q --sort=yes

