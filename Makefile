CC = gcc
CFLAGS = -Wall -g -pthread
.PHONY: default
default: all

all: narrow_road.o random.c
	$(CC) $(CFLAGS) $^ -o $@

%.o:%.c
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	-rm -f *.exe *.o