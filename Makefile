.PHONY: all indent clean

# Default value for Linux-like systems.
FREETYPE_PREFIX=/usr
BREW_FREETYPE=$(shell brew --prefix freetype2 2>/dev/null)
ifneq ($(BREW_FREETYPE),)
	FREETYPE_PREFIX := ${BREW_FREETYPE}
endif

C=gcc
CPP=g++
CFLAGS=-Wall -W -O3 -I${FREETYPE_PREFIX}/include/freetype2
CPPFLAGS=-std=c++11 ${CFLAGS}
LDFLAGS=-L${FREETYPE_PREFIX}/lib -lfreetype
SRC=$(wildcard *.cc)
BIN=$(SRC:%.cc=build/%)

all: build ${BIN}

run:
	build/ttf-unicode-ranges

indent:
	(find . -name "*.cc" -o -name "*.h") | xargs clang-format -i

clean:
	rm -rf build

build:
	mkdir -p $@

build/%: %.cc
	${CPP} ${CPPFLAGS} -o $@ $< ${LDFLAGS}
