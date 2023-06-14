CC=cc
CFLAGS=-Wall -Werror -pedantic -g -DDEBUG
CFILES=$(wildcard src/*.c)
HFILES=$(wildcard src/*.h)

all: 
	@$(CC) $(HFILES) $(CFILES) $(CFLAGS) -o server 
