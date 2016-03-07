CC = gcc
CFLAGS = -g -Wall -Wextra -Wno-unused -Werror

SOURCES = main.c
OBJECTS = $(subst .c,.o,$(SOURCES))

default: addtest

addtest: $(OBJECTS)
	$(CC) $(FLAGS) -o $@ $(OBJECTS)