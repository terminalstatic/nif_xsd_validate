CC = gcc
ERLANG_PATH=$(shell erl -eval 'io:format("~s", [lists:concat([code:root_dir(), "/erts-", erlang:system_info(version), "/include"])])' -s init stop -noshell)
CFLAGS=-fPIC -I$(ERLANG_PATH) -I/usr/local/include -I/usr/include -L/usr/local/lib -L/usr/lib -std=c99 -O2 -Wall -pedantic -g  $(shell pkg-config --cflags libxml-2.0)


.PHONY: all clean
priv/validateXsd.so: c_source/validateXsd.c c_source/vlibxml.h priv/vlibxml.o
	$(CC) $(CFLAGS) -shared -o $@ c_source/validateXsd.c priv/vlibxml.o $(shell pkg-config --libs libxml-2.0)

priv/vlibxml.o: c_source/vlibxml.c c_source/vlibxml.h
	$(CC) $(CFLAGS) -c -o $@ c_source/vlibxml.c 

clean:
	rm priv/*.so priv/*.o
