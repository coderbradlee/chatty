
C=clang
CFLAGS=-c -g -std=c99
LDFLAGS=

all: chatty

chatty: *.o
	$(C) $(LDFLAGS) $< -o $@

%.o: %.c *.h Makefile
	$(C) $(CFLAGS) $< -o $@

clean:
	rm -f *.o
