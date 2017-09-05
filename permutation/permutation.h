/*
 * PERMUTE THE VARIABLES NOT TO GET CROSSING TRAJECTORIES
 * AND COMPUTE #V(x_i) FOR THE j FIRST CLAUSES
 *
 * This file contains the declarations of the functions defined
 * in the file permutation.c
 *
 * Author: M. Remon
 * Version: 5 Aug 2016
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>

#define DEBUG 0    // debug parameter, an integer in [0, 3], if set to 0
                   // then no debug information will be printed

/* This function trims leading and trailing blanks in a string
 * stored as an array of char.
 *
 * Param:
 * - s (in/out) : the string to be modified
 */
void trim(char *s);

/* This function relabels the litterals in a cnf file and save the 
 * results in a new cnf file.
 *
 * Param:
 * - filename (in): a string detailing the path to the input cnf file
 * - sorted (in/out): an array containing the updated sorting of the litterals
 * - out (out): the resulting adjacent matrix from the relabelling proceess
 * - n_row (out) and n_col (out): pointers to integers keeping track of the 
 *                                number of rows and columns in out
 * - litteral_i (out): an array containing the index of the first/second/third
 *                     litteral of every clause (i = 1, 2, 3)
 */

void read_input(char *filename, int** out, int *dim_x, int *dim_y);

/* This function prints a matrix on the screen.
 *
 * Param:
 * - matrix (in): the matrix containing the data to be printed
 * - n_row (in): the number of rows of the matrix
 * - n_col (in): the number of columns of the matrix
 */
void print_matrix(int** matrix, size_t n_row, size_t n_col);

/* This function checks if there is a path between 
 * the litterals i and j in an adjacency matrix
 *
 * Param:
 * - matrix (in): an adjacency matrix
 * - i (in): id of litteral i
 * - j (in): id of litteral j
 *
 * Return: an int, set to 1 if there is a path, 0 otherwise.
 */
int is_path(int** matrix, int i, int j);
