CC = gcc
CFLAGS = -g -Wall -Wextra -Wno-unused
LAB = 1
DISTDIR = lab1-EugeneLiu_CameronIto

SIMPSH_SOURCES = fileoptions.c main.c
SIMPSH_OBJECTS = $(subst .c,.o,$(SIMPSH_SOURCES))

DIST_SOURCES = $(SIMPSH_SOURCES) fileoptions.h Makefile

default: simpsh

simpsh: $(SIMPSH_OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(SIMPSH_OBJECTS)

fileoptions.o: fileoptions.h

dist: $(DISTDIR).tar.gz

$(DISTDIR).tar.gz: $(DIST_SOURCES)
	rm -rf $(DISTDIR)
	tar -czf $@ --transform='s/^/$(DISTDIR)//' $(DIST_SOURCES)	

clean:
	rm -rf *.o *.tar.gz *.tmp $(DISTDIR)
	
check: simpsh
	./test.sh