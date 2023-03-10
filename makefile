CC = gcc
CXX = g++
CFLAGS = -Wall -g
PREFIX = /usr/local

build:
	$(CXX) $(CFLAGS) main.cpp -o serial-cli

install:
	mkdir -p $(DESTDIR)$(PREFIX)/bin/
	cp ./serial-cli $(DESTDIR)$(PREFIX)/bin/serial-cli

uninstall:
	rm -f $(PREFIX)/bin/serial-cli
