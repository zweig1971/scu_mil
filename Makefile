# PREFIX  controls where programs and libraries get installed
# Example usage:
#   make PREFIX=/usr all

PWD     := $(shell pwd)
HOME    = $(PWD)/..
ODIR    = $(HOME)/scu_mil
IDIR    = $(HOME)/scu_mil
PREFIX  ?= /usr/local
LIBS    = -letherbone
FLAG    = DONTSIMULATE

all:
	g++ -g -Wall -D$(FLAG) $(INCLUDE) -Wl,-rpath,$(PREFIX)/lib -o $(ODIR)/scumil $(IDIR)/scumil.cpp $(IDIR)/main.cpp $(LIBS) #$(IDIR)/dow_crc.c $(LIBS)

clean:
	@echo cleaning stuff
	rm -f scumil

