CC=gcc
CFLAGS=-g

SRCS := $(wildcard *.c)

all: main

main: $(OBJS)
	$(CC) $(CFLAGS) $(SRCS) -o main

clean:
	rm main