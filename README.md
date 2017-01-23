# santabag

This repository contains my C code for solving the [Santa's Uncertain Bags](https://www.kaggle.com/c/santas-uncertain-bags)
competition at Kaggle.

It currently contains three tools and some common code.

## Tools
 * checker - a tool that lakes a submission file and runs a MC simulations of it.
 * simulate_bags - a tool that simulates all possible bag combinations of toy types.
 * santabag_glpk - a tool that solved the MIP problem and writes out a solution.

## Dependencies
 * checker and simulate_bags depends on [GNU Scientific Library (GSL)](https://www.gnu.org/software/gsl/)
 * santabag_glpk depends on [GLPK](https://www.gnu.org/software/glpk/)

## Build
There is a Makefile in the repository. It is really plain simple. If you have installed GLPK and GSL in
default paths, I guess typing `make` should build the tools. I've only tried GCC but the code is pretty standard C99,
so if you use the GCC on command line remember to set `-std=c99` or `-std=gnu99`. I guess other compilers will work as well.

## Threading
The simulating tools uses [OpenMP](http://www.openmp.org/) to do naïve multithreading. If you do not want multithreading
or do not want support for this, simply remove the `-fopenmp` compile flag and the `-lgomp` link library. (And
ignore the pragma warning)
