#C=gcc
#CFLAGS=-c -g -O0
C=clang
CFLAGS=-c -g -O0 \
	   -Wno-incompatible-pointer-types-discards-qualifiers \
	   -Wno-visibility \
	   -Wno-incompatible-pointer-types
LDFLAGS=-lpthread #-ltcmalloc

all: main jsonc-test ws-test


#byz.o ecpoll.o chatty.o ws.o

main: main.o chatty.o byz.o ws.o sha1.o base64.o ecpoll.o
	$(C) $(LDFLAGS) $^ -o $@

jsonc-test: jsonc-test.o
	$(C) -ljson-c $^ -o $@

ws-test: ws-test.o sha1.o base64.o ws.o
	$(C) $^ -o $@

%.o: %.c *.h Makefile
	$(C) $(CFLAGS) $< -o $@

clean:
	rm -f *.o
	rm main jsonc-test
