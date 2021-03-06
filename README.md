# santabag

This repository contains my C code for solving the [Santa's Uncertain Bags](https://www.kaggle.com/c/santas-uncertain-bags)
competition at Kaggle.

It currently contains three tools and some common code.

## Tools
 * checker - a tool that takes a submission file and runs a MC simulations of it.
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
The simulating tools uses [OpenMP](http://www.openmp.org/) to do naïve multithreading. You therefore need a compiler that
supports OpenMP (Most modern C compilers does). ~~If you do not want multithreading
or do not want support for this, simply remove the `-fopenmp` compile flag and the `-lgomp` link library. (And
ignore the pragma warning)~~

## Bugs
Oh my! I just realized that the $+mu + \alpha \sigma$ logic may be wrong! Please use $\alpha=0.0$ while I check
how I could make such a mistake.

## TODO
Add lpsolve code. Code cleanup. Use better command line options for the tools. (getopt?)

## Example usage

    $ git clone https://github.com/oysteijo/santabag.git
    $ cd santabag
    $ make
    $ ./simulate_bags 1000000
    $ ./santabag_glpk simulations.csv

That should create a submission file `submission.csv` which can be submittet to the competition.
