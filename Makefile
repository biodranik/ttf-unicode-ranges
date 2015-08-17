.PHONY: all indent clean

# This one is for Mac OS X Yosemite
FREETYPE_INCLUDE_DIR=/opt/X11/include/freetype2
FREETYPE_LIB_DIR=/opt/X11/lib

C=gcc
CPP=g++
CFLAGS=-Wall -W -O3 -I${FREETYPE_INCLUDE_DIR}
CPPFLAGS=-std=c++11 ${CFLAGS}
LDFLAGS=-L${FREETYPE_LIB_DIR} -lfreetype
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

