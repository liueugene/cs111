CC = gcc
CFLAGS = -g -Wall -Wextra -Wno-unused -Werror -pthread

SOURCES = main.c
OBJECTS = $(subst .c,.o,$(SOURCES))

default: addtest

addtest: $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS)