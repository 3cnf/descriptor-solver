# A multi-linear descriptor approach to solve 3-CNF SAT problems

This repository contains the implementation of the algorithms detailed in the paper
[A 3-CNF-SAT descriptor algebra and the solution of the P=NP conjecture][https://arxiv.org/abs/1609.05709].

It consists of the following sub-programs:
- `algo` that implements the core algorithm to solve 3-CNF SAT problems
- `check_solution` verifying if the solutions found by the algorithm are true solutions of a 3-CNF problem;
- `compute_W` approximates the complexity of the core algorithm;
- `combine_H` combines H matrices;
- `filter` is used to find a sub-problem of a given maximal complexity for the original 3-CNF problem. Note that to obtain different sub-problems, the filter should be applied on different subset of the original cnf file;
- `permutation` that handles the relabelling of the variables and the permutation of the clauses as described in the paper;
- and `toughsat` written by Henry Yuen for the [ToughSat Project][https://toughsat.appspot.com/] to generate 3-CNF problems.

In addition, you can find sample problems in the `examples` directory.

## How to compile the code and requirements

Some programs are written in C or C++11 and thus to be compiled with GCC or G++ before they can be used:

- `algo` : `g++ cnf.cpp -o cnf -std=c++11 -lgmpxx -lgmp -lm -Wall -O3`
- `permutation` : `gcc permutation.c -o permutation`

Those programs has been sucessfully compiled with GCC 7.1.1, but may compile with other versions. The C++ code needs the Boost and GMP libraries.

The remaining of the programs and scripts are written in Python 3 or Bash and do not require any compilation.

## How to use the programs to solve a 3-CNF SAT problem

Let us assume that we have a cnf file containing a 3-CNF SAT problem: `test.cnf`.

1. Use the permutation program: `permutation test.cnf > test_sorted.cnf`.
2. Approximate the complexity of the problem (optional): `python3 compute_W.py test_sorted.cnf` (the output *n* corresponds to a complexity of *2^n*).
3. If the complexity is low enough (e.g. less than 2^**20**) compute the solutions: `cnf test_sorted.cnf`. The solutions will be saved in the file `test_sorted.cnf.sol` and the matrix H in `test_sorted.cnf.Hmat`.
4. If the complexity is too high, find a sub-problem of a given complity (e.g. 2^**20**): `sh filter_clauses.sh test_sorted.cnf 20`. The sub-problem will be saved in the file `test_sorted.cnf.up`.
5. Solve the sub-problem: `cnf test_sorted.cnf.up`
6. In order to find the solutions of the sub-problem that are also solutions of the original problem: `python3 check_solution.py test_sorted.cnf test_sorted.cnf.up.sol`
7. Combine the H matrices of several sub-problems into one: `sh combine.sh`. The output can be saved, e.g. in `combined.Hmat`.
8. Solve the combined sub-problems: `cnf combined.Hmat neutral.vec n_vars` where `n_vars` is the number of variables and `neutral.vec` is a one-line file containing `n_vars 1`, e.g. `6 1` for a problem with 6 variables. The solution will be saved in `combined.Hmat.sol`.

See the script `run_test.sh` for more details.
