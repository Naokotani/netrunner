CC=gcc
BINARY=netrunner
CFILES=main.c
OBJECTS=main.o
LIB=-lncurses
CFLAGS= -Wall -Wextra

all: $(BINARY)

$(BINARY): $(OBJECTS)
	$(CC) $(LIB) -o $@ $^

%.o:%.c
	$(CC) $(CFLAGS) -c -o $@ $^

clean:
	rm netrunner $(OBJECTS)
