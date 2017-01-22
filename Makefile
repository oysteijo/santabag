CC      = gcc
OPT     = -std=gnu99 -Wall -Wextra -O3 -fopenmp
INCLUDE = -I. -I/usr/include/lpsolve
LIBS    = -lgsl -lcblas -lgomp -lm
CFLAGS  = $(OPT) $(INCLUDE)

SRC     = $(wildcard *.c)
OBJS    = $(patsubst %.c,%.o,$(SRC))

all    : checker simulate_bags santabag_glpk
checker: checker.o toyrandom.o
	$(CC) -o $@ $^ $(LIBS)
simulate_bags: simulate_bags.o toyrandom.o
	$(CC) -o $@ $^ $(LIBS)
santabag_glpk: santabag_glpk.o
	$(CC) -o $@ $^ -lglpk -lm
	
.PHONY :	clean 
clean:
	$(RM) $(OBJS) *~

