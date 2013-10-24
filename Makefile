
C=clang
CFLAGS=-c -g -std=c99
LDFLAGS=-ltcmalloc

all: chatty

chatty: chattyio.o websocket.o global.o main.o bbuf.o
	$(C) $(LDFLAGS) $^ -o $@

%.o: %.c *.h Makefile
	$(C) $(CFLAGS) $< -o $@

clean:
	rm -f *.o
