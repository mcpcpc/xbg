.POSIX:
CC     = cc
CFLAGS = -W -O
PREFIX = /usr/local
LDLIBS = -lm
ALL_LDFLAGS = $(LDFLAGS) $(LIBS) -lxcb -lxcb-util

all: xbg
install: all
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f xbg $(DESTDIR)$(PREFIX)/bin
	chmod 755 $(DESTDIR)$(PREFIX)/bin/xbg
xbg: xbg.o
	$(CC) $(ALL_LDFLAGS) -o xbg xbg.o $(LDLIBS)
xbg.o: xbg.c
clean:
	rm -f xbg *.o
uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/xbg
.PHONY: all install uninstall clean
