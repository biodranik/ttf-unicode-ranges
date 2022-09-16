.PHONY: all indent clean

UNAME := $(shell uname)
ifeq ($(UNAME), Darwin)
    # This one is for Mac OS X Yosemite
    FREETYPE_INCLUDE_DIR=/opt/X11/include/freetype2
    FREETYPE_LIB_DIR=/opt/X11/lib
endif
ifeq ($(UNAME), Linux)
    FREETYPE_INCLUDE_DIR=/usr/include/freetype2
    FREETYPE_LIB_DIR=/usr/lib
endif

C=gcc
CPP=g++
CFLAGS=-Wall -W -O0 -g -I${FREETYPE_INCLUDE_DIR}
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

