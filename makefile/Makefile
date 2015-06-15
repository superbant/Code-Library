all:test

test: test.o print.o lookup.o
	gcc $^ -o $@ 

clean:
	@echo "=========cleanning project"
	@-rm test lookup print  *.o
	@echo "=========clean complete"

.PHONY: clean all

sources = test.c print.c lookup.c

include $(sources:.c=.d)

%.d: %.c
	set -e; rm -f $@; \
	$(CC) -MM $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@: ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$
