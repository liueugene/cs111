CC = gcc
CFLAGS = -g -Wall -Wextra -Wno-unused -Werror -pthread

ADDTEST_SOURCES = addtest.c
ADDTEST_OBJECTS = $(subst .c,.o,$(ADDTEST_SOURCES))
SLTEST_SOURCES = SortedList.c sltest.c
SLTEST_OBJECTS = $(subst .c,.o,$(SLTEST_SOURCES))

default: addtest sltest

addtest: $(ADDTEST_OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(ADDTEST_OBJECTS)
	
sltest: $(SLTEST_OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(SLTEST_OBJECTS) 

SortedList.o: SortedList.h