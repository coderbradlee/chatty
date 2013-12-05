#C=gcc
#CFLAGS=-c -g -O0
C=clang
CFLAGS=-c -g -O0 \
	   -Wno-incompatible-pointer-types-discards-qualifiers \
	   -Wno-visibility \
	   -Wno-incompatible-pointer-types
LDFLAGS=-lpthread #-ltcmalloc

all: main jsonc-test

main: main.o byz.o ecpoll.o chatty.o ws.o
	$(C) $(LDFLAGS) $^ -o $@

jsonc-test: jsonc-test.o sha1.o base64.o
	$(C) -ljson-c $^ -o $@

%.o: %.c *.h Makefile
	$(C) $(CFLAGS) $< -o $@

clean:
	rm -f *.o
	rm main jsonc-test
