#C=gcc
#CFLAGS=-c -g -O0
C=clang
CFLAGS=-c -g -O0 \
	   -Wno-incompatible-pointer-types-discards-qualifiers \
	   -Wno-visibility \
	   -Wno-incompatible-pointer-types
LDFLAGS=-lpthread #-ltcmalloc

all: main

main: main.o byz.o ecpoll.o chatty.o
	$(C) $(LDFLAGS) $^ -o $@

%.o: %.c *.h Makefile
	$(C) $(CFLAGS) $< -o $@

clean:
	rm -f *.o
	rm test
