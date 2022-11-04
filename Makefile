CC = gcc
CFLAGS = -Wall -g -pthread
.PHONY: road
all: road

road: narrow_road.o random.c
	$(CC) $(CFLAGS) $^ -o $@.exe

%.o:%.c
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	-rm -f *.exe *.o
