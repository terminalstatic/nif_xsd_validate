CC = gcc
LIBXML2 = libxml2-2.9.8
ERLANG_PATH=$(shell erl -eval 'io:format("~s", [lists:concat([code:root_dir(), "/erts-", erlang:system_info(version), "/include"])])' -s init stop -noshell)
CFLAGS=-fPIC -I$(ERLANG_PATH) -Ipriv/libxml2/include/libxml2 -Lpriv/libxml2/lib -std=c99 -O2 -Wall -pedantic -g  $(shell pkg-config --cflags libxml-2.0)
CURL = curl -LO

ifneq ($(OS),Windows_NT)
	CFLAGS += -fPIC

	ifeq ($(shell uname),Darwin)
		LDFLAGS += -dynamiclib -undefined dynamic_lookup
	endif
endif

.PHONY: all clean

priv/nif_xsd_validate.so: c_source/nif_xsd_validate.c priv/libxml2/lib/libxml2.a c_source/vlibxml.h priv/vlibxml.o
	mkdir -p priv
	$(CC) $(CFLAGS) -shared $(LDFLAGS) -o $@ c_source/nif_xsd_validate.c priv/libxml2/lib/libxml2.a priv/vlibxml.o $(shell pkg-config --libs libxml-2.0)
	@rm -f priv/vlibxml.o

priv/vlibxml.o: c_source/vlibxml.c c_source/vlibxml.h
	mkdir -p priv
	$(CC) $(CFLAGS) -c -o $@ c_source/vlibxml.c 

priv/libxml2/lib/libxml2.a:
	rm -rf c_build
	mkdir -p c_build
	cd c_build \
		&& curl -LO ftp://xmlsoft.org/libxml2/$(LIBXML2).tar.gz \
		&& tar -xf $(LIBXML2).tar.gz \
		&& cd $(LIBXML2) \
		&& ./configure --prefix=`pwd`/../../priv/libxml2 --with-pic --without-python --with-threads --with-history \
		&& make \
		&& make install
	rm -rf c_build
	rm -rf priv/libxml2/bin
	rm -rf priv/libxml2/include
	rm -rf priv/libxml2/share
clean:
	rm -f priv/*.so
	@rm -rf c_build
	@rm -rf priv/libxml2
