DISCRETA_CPP = g++
DISCRETA_CC = gcc
CPPFLAGS = -Wall -O3 -march=native -funroll-loops -I..
# -frename-registers
LIB = 
LFLAGS = -lm

MODULES = \
	galois_global.o \
	magma_interface.o \
	numerics.o 


all : $(MODULES)

clean:
	- rm *.o *.a *~

