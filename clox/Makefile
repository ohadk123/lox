CC=gcc
CFLAGS=-g -O0

SRCS := $(wildcard *.c)

all: main

main: $(OBJS)
	$(CC) $(CFLAGS) $(SRCS) -o main

clean:
	rm main
