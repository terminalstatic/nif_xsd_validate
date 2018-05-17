CC = gcc
ERLANG_PATH=$(shell erl -eval 'io:format("~s", [lists:concat([code:root_dir(), "/erts-", erlang:system_info(version), "/include"])])' -s init stop -noshell)
CFLAGS=-fPIC -I$(ERLANG_PATH) -I/usr/local/include -I/usr/include -L/usr/local/lib -L/usr/lib -std=c99 -O2 -Wall -pedantic -g  $(shell pkg-config --cflags libxml-2.0)
CURL = curl -LO

ifneq ($(OS),Windows_NT)
	CFLAGS += -fPIC

	ifeq ($(shell uname),Darwin)
		LDFLAGS += -dynamiclib -undefined dynamic_lookup
	endif
endif

.PHONY: all clean

priv/nif_xsd_validate.so: c_source/nif_xsd_validate.c c_source/vlibxml.h priv/vlibxml.o priv/libxml2/lib/libxml2.a
	mkdir -p priv
	$(CC) $(CFLAGS) -shared $(LDFLAGS) -o $@ c_source/nif_xsd_validate.c priv/vlibxml.o priv/libxml2/lib/libxml2.a $(shell pkg-config --libs libxml-2.0)
	@rm -f priv/vlibxml.o

priv/vlibxml.o: c_source/vlibxml.c c_source/vlibxml.h
	mkdir -p priv
	$(CC) $(CFLAGS) -c -o $@ c_source/vlibxml.c 

priv/libxml2/lib/libxml2.a:
	@rm -rf c_build
	mkdir -p c_build
	cd c_build \
		&& curl -sL ftp://xmlsoft.org/libxml2/libxml2-2.9.8.tar.gz| tar -xzf - \
		&& cd libxml2-2.9.8 \
		&& ./configure --prefix=`pwd`/../../priv/libxml2 --with-pic --without-python --enable-static --with-threads --with-history \
		&& make \
		&& make install
	rm -rf c_build
clean:
	rm -f priv/*.so
	@rm -rf c_build
	@rm -rf priv/libxml2
