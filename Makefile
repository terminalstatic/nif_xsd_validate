CC = gcc
ERLANG_PATH=$(shell erl -eval 'io:format("~s", [lists:concat([code:root_dir(), "/erts-", erlang:system_info(version), "/include"])])' -s init stop -noshell)
CFLAGS=-fPIC -I$(ERLANG_PATH) -Ipriv/libxml2/include/libxml2 -Lpriv/libxml2/lib -std=c99 -O2 -Wall -pedantic -g  $(shell pkg-config --cflags libxml-2.0)

ifeq ($(shell uname),Darwin)
	LDFLAGS += -dynamiclib -undefined dynamic_lookup
endif

.PHONY: all clean

priv/nif_xsd_validate.so: c_source/nif_xsd_validate.c priv/libxml2/lib/libxml2.a priv/vlibxml.o
	@mkdir -p priv
	@$(CC) $(CFLAGS) -shared $(LDFLAGS) -o $@ $+ $(shell pkg-config --libs libxml-2.0)
	@rm -f priv/vlibxml.o

priv/vlibxml.o: c_source/vlibxml.c c_source/vlibxml.h
	@mkdir -p priv
	@$(CC) $(CFLAGS) -c -o $@ $< 

priv/libxml2/lib/libxml2.a:
	rm -rf c_build
	mkdir -p c_build
	curl -L ftp://xmlsoft.org/libxml2/LATEST_LIBXML2 -o ./c_build/LATEST_LIBXML2.tar.gz 
	tar -xf c_build/LATEST_LIBXML2.tar.gz -C ./c_build/
	cd ./c_build/libxml2* \
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
