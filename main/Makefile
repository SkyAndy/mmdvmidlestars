# Makefile itself dir
ROOT_DIR:=$(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))

# hw platform as from autogen.sh choose
HWPLAT:=$(shell cat $(ROOT_DIR)/../hwplatform)

# sets CCFLAGS hw platform dependant
ifeq ($(HWPLAT),BananaPI)
        CCFLAGS=-Wall -Ofast -mfpu=vfpv4 -mfloat-abi=hard -march=armv7 -mtune=cortex-a7 -DBANANAPI
else # fallback to raspberry
        # The recommended compiler flags for the Raspberry Pi
        CCFLAGS=-Ofast -mfpu=vfp -mfloat-abi=hard -march=armv6zk -mtune=arm1176jzf-s
endif

prefix := /usr/local

# define all programs
PROGRAMS = mmdvmidlestars
SOURCES = ${PROGRAMS:=.cpp}

CXX=g++
CFLAGS=${CCFLAGS}

all: ${PROGRAMS}

${PROGRAMS}: ${SOURCES}
        $(CXX) $(CFLAGS) -Wall  -lArduiPi_OLED $@.cpp -o $@

clean:
        rm -rf $(PROGRAMS)

install: all
        test -d $(prefix) || mkdir $(prefix)
        test -d $(prefix)/bin || mkdir $(prefix)/bin
        for prog in $(PROGRAMS); do \
          install -m 0755 $$prog $(prefix)/bin; \
        done

.PHONY: install
