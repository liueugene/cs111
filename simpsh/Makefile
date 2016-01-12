CC = gcc
CFLAGS = -g -Wall -Wextra -Wno-unused
LAB = 1
DISTDIR = lab1-$(USER)

TESTS = $(wildcard test*.sh)
TEST_BASES = $(subst .sh,,$(TESTS))

SIMPSH_SOURCES = fileoptions.c main.c
SIMPSH_OBJECTS = $(subst .c,.o,$(SIMPSH_SOURCES))

DIST_SOURCES = $(SIMPSH_SOURCES) fileoptions.h Makefile

default: simpsh

simpsh: $(SIMPSH_OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(SIMPSH_OBJECTS)

fileoptions.o: fileoptions.h

dist: $(DISTDIR).tar.gz

$(DISTDIR).tar.gz: $(DIST_SOURCES) check-dist
	rm -fr $(DISTDIR)
	tar -czf $@.tmp --transform='s,^,$(DISTDIR)/,' $(DIST_SOURCES)
	./check-dist $(DISTDIR)
	mv $@.tmp $@
	

clean:
	rm -fr *.o *~ *.bak *.tar.gz core *.core *.tmp $(DISTDIR)
	
check: $(TEST_BASES)

$(TEST_BASES): simpsh
	./$@.sh
	

.PHONY: default dist clean