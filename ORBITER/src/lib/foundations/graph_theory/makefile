DISCRETA_CPP = g++
DISCRETA_CC = gcc
CPPFLAGS = -Wall -O3 -march=native -funroll-loops -I..
# -frename-registers
LIB = 
LFLAGS = -lm

MODULES = \
	clique_finder.o \
	clique_finder_control.o \
	colored_graph.o \
	graph_layer.o \
	graph_node.o \
	layered_graph.o \
	layered_graph_draw_options.o \
	rainbow_cliques.o 


all : $(MODULES)

clean:
	- rm *.o *.a *~
