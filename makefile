CC=gcc
BINARY=netrunner
CFILES=main.c
OBJECTS=main.o move.o box.o wall.o menu.o save.o
LIB=-lncurses
CFLAGS= -Wall -Wextra -g

all: $(BINARY)

$(BINARY): $(OBJECTS)
	$(CC) $(LIB) -o $@ $^

%.o:%.c
	$(CC) $(CFLAGS) -c -o $@ $^

clean:
	rm netrunner $(OBJECTS)
