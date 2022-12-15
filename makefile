CC = g++
CFLAGS = -Wall -g
PREFIX = /usr/local

build:
	$(CC) $(CFLAGS) main.cpp -o serial-cli

install:
	cp ./serial-cli $(DESTDIR)$(PREFIX)/bin/serial-cli

uninstall:
	rm -f $(PREFIX)/bin/serial-cli
