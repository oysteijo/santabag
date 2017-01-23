# santabag

This repository contains my C code for solving the Santa Uncertan Bags competition at Kaggle.

It currently contains three tools and some common code.

## Tools:
 * checker - a tool that lakes a submission file and runs a MC simulations of it.
 * simulate_bags - a tool that simulates all possible bag combinations of toy types.
 * santabag_glpk - a tool that solved the MIP problem and writes out a solution.

## Dependencies:
 * checker and simulate_bags depends on [GNU Scientific Library (GSL)](https://www.gnu.org/software/gsl/)
 * santabag_glpk depends on [GLPK](https://www.gnu.org/software/glpk/)
