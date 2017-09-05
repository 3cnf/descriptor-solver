/*
 * PERMUTE THE VARIABLES AS EXPLAINED IN THE PAPER
 * SO THAT MOST FREQUENT VARIABLES HAVE SMALLER INDICES.
 *
 * This file contains the definitions of the functions declared
 * in the file permutation.h
 *
 * How to compile: gcc permutation.c -o permutation
 *
 * Author: M. Remon
 * Version: 30 Aug 2017
 */

#include <stdlib.h>
#include "permutation.h"

// Derives a contingency matrix of the litterals from a standart cnf file.
void read_input(char *filename, int** out, int *n_cl, int *n_lit) {

  // dimensions of the cnf problem
  int n_litterals, n_clauses;

  // opening the cnf file and finding the dimension of the matrix
  char str[256];
  FILE * f;
  f = fopen(filename, "r");
  if (f == NULL) {
    printf("Error: could not open the file %s!\n", filename);
    exit(EXIT_FAILURE);
  }

  // loop until find the right line with the problem description
  do {
    fgets(str, 256, f);
    trim(str);
  } while (str[0] != 'p');
  
  // extracting the problem description from line 'p cnf n_var n_clauses' in the cnf file
  sscanf(str, "%*s %*s %d %d", &n_litterals, &n_clauses);
  if( DEBUG == 1) {
    printf("DIMENSION OF THE PROBLEM: litterals: %d, clauses: %d\n", n_litterals, n_clauses);
  }
    
  // memory allocation
  *out = calloc(n_litterals, sizeof(int *));
 
  // reading data and storing it in out
  int lit_1, lit_2, lit_3, lit_temp;
  int clause = 0;
  while (fscanf(f, "%d %d %d %*s", &lit_1, &lit_2, &lit_3) == 3 && clause < n_clauses) {

    // sorting the litterals
    if (abs(lit_1) < abs(lit_2)) {
      lit_temp = lit_1;
      lit_1    = lit_2;
      lit_2    = lit_temp;
    }
    if (abs(lit_2) < abs(lit_3)) {
      lit_temp = lit_2;
      lit_2    = lit_3;
      lit_3    = lit_temp;
    }
    if (abs(lit_1) < abs(lit_2)) {
      lit_temp = lit_1;
      lit_1    = lit_2;
      lit_2    = lit_temp;
    }

    // updating the frequency of the litterals
    (*out)[abs(lit_1) - 1]++;
    (*out)[abs(lit_2) - 1]++;
    (*out)[abs(lit_3) - 1]++;

    if (DEBUG == 1) {
      printf("SORTED CLAUSE %d: %d %d %d\n", clause + 1, lit_1, lit_2, lit_3);
      printf("Frequency[%d]=%d\n", abs(lit_1), (*out)[abs(lit_1) - 1]);
      printf("Frequency[%d]=%d\n", abs(lit_2), (*out)[abs(lit_2) - 1]);
      printf("Frequency[%d]=%d\n", abs(lit_3), (*out)[abs(lit_3) - 1]);
    }

  }

  // closing the file
  fclose(f);  

  // saving the dimension of the matrix
  *n_cl = n_clauses; 
  *n_lit = n_litterals;
  
}


// Trimming a string.
void trim(char * s) {

  char *p = s;
  int l = strlen(p);
  while (isspace(p[l-1])) p[--l] = 0;
  while (*p && isspace(*p)) {
    ++p;
    --l;
  }
  memmove(s, p, l + 1);

}

// Relabelling the litterals and saving the new cnf file.
void write_relabel(char *filename, int* sorted, int** out, int *n_row, int *n_col,
		   int **litteral_1, int **litteral_2, int **litteral_3) {

  // variables keeping track of the dimension of the problem
  int n_clauses;
  int n_litterals;

  // opening the input cnf file
  char str[256];
  FILE * f;
  f = fopen(filename, "r");
  if (f == NULL) {
    printf("Error: could not open the file %s!\n", filename);
    exit(EXIT_FAILURE);
  }

  // loop until find the right line with the problem description
  do {
    fgets(str, 256, f);
    trim(str);
    // printf("READ: %s \n", str);
  } while (str[0] != 'p');
  
  // extracting the problem dimension (p cnf n_var n_clauses)
  sscanf(str, "%*s %*s %d %d", &n_litterals, &n_clauses);
  *n_row = n_clauses;
  *n_col = n_litterals;

  // reading data
  int lit_1, lit_2, lit_3, lit_temp;
  int new_lit_1, new_lit_2, new_lit_3;
  int clause = 0;
  fgets(str, 256, f);
  trim(str);  
  while (sscanf(str, "%d %d %d %*s", &lit_1, &lit_2, &lit_3) == 3
	 && clause < n_clauses) {

    if (DEBUG == 2) {    
      printf("READ: %s", str);
    }
      
    if (DEBUG == 3) {
      printf("DEBUG : %d %d %d \n", lit_1, lit_2, lit_3);
    }

    // relabelling the litterals   
    if (lit_1 > 0) {
      new_lit_1 = sorted[lit_1 - 1];
    } else {
      new_lit_1 = -sorted[abs(lit_1) - 1];
    }
    if (lit_2 > 0) {
      new_lit_2 = sorted[lit_2 - 1];
    } else {
      new_lit_2 = -sorted[abs(lit_2) - 1];
    }
    if (lit_3 > 0) {
      new_lit_3 = sorted[lit_3 - 1];
    } else {
      new_lit_3 = -sorted[abs(lit_3) - 1];
    }
    if (DEBUG == 1) {
      printf("SORTED CLAUSE %d: %d[%d] %d[%d] %d[%d] \n",
	     clause + 1, new_lit_1, lit_1, new_lit_2, lit_2, new_lit_3, lit_3);
    }

    // sorting the new litterals by increasing index order
    if (abs(new_lit_1) < abs(new_lit_2)) {
      lit_temp  = new_lit_1;
      new_lit_1 = new_lit_2;
      new_lit_2 = lit_temp;
    }
    if (abs(new_lit_2) < abs(new_lit_3)) {
      lit_temp  = new_lit_2;
      new_lit_2 = new_lit_3;
      new_lit_3 = lit_temp;
    }
    if (abs(new_lit_1) < abs(new_lit_2)) {
      lit_temp  = new_lit_1;
      new_lit_1 = new_lit_2;
      new_lit_2 = lit_temp;
    }
     
    (*litteral_1)[clause] = new_lit_1;
    (*litteral_2)[clause] = new_lit_2;
    (*litteral_3)[clause] = new_lit_3;

    if (DEBUG == 1) {
      printf("NEW LITTERALS CLAUSE %d: %d %d %d 0 \n",
	     clause + 1, new_lit_1, new_lit_2, new_lit_3);
    }


    // reading the next clause
    clause++;
    fgets(str, 256, f);
    trim(str);
    
  }

  // closing the file
  fclose(f);  

  // write the clause in descending order

  for (int i=n_litterals;i>2;i--){
    for (clause=1;clause<=n_clauses;clause++){
      if ((*litteral_1)[clause] == i){
	printf(" %d %d %d 0 \n",(*litteral_1)[clause],(*litteral_2)[clause],(*litteral_3)[clause]);
      }
    }
    for (clause=1;clause<=n_clauses;clause++){
      if ((*litteral_1)[clause] == -i){
	printf(" %d %d %d 0 \n",(*litteral_1)[clause],(*litteral_2)[clause],(*litteral_3)[clause]);
      }
    }
  }


  
}

  
int main(int argc, char* argv[]) {

  if (argc != 2){
    printf("Usage : permutation input_file.cnf > output_file.cnf \n");
    return EXIT_SUCCESS;
  }
  
  // read cnf input file
  char *input_filename = argv[1];
  int *freq_matrix;
  int n_clauses, n_litterals; 
  if (DEBUG == 1) {
    printf("input file: %s\n", input_filename);
  }
  read_input(input_filename, &freq_matrix, &n_clauses, &n_litterals);
  
  // printing input data
  if (DEBUG == 1) {
    for (int i = 0; i < n_litterals; i++) {
      printf("Frequency[%d]=%d\n", i + 1, freq_matrix[i]);
    }
  }

  // sorting the litterals by ascending order of frequency in the problem
  int *sorted;
  sorted = calloc(n_litterals, sizeof(int *));  
  int k = 0;
  int index_min = 0;
  int min_freq  = n_clauses+1;
  while (k < n_litterals) {  
    for (int i = 0; i < n_litterals; i++) {
      if (freq_matrix[i] < min_freq) {
	min_freq = freq_matrix[i] ;
	index_min = i;
      }
    }   
    
    if (sorted[index_min] == 0) {
      sorted[index_min] = n_litterals - k;
      //      printf("k=%d index=%d \n", k, index_min + 1);
      k++;
    }
    
    freq_matrix[index_min] = n_clauses + 1;
    min_freq = n_clauses + 1;
  }
    
  if (DEBUG == 1) {
    for (int i=0;i<n_litterals;i++) {
      printf("i=%d -> j=%d \n",i+1,sorted[i]);
    }
  }

  // ... allocating memory for the arrays storing the first, second and third litteral of each clause
  int *litteral_1, *litteral_2, *litteral_3;
  litteral_1   = calloc(n_clauses, sizeof(int));
  litteral_2   = calloc(n_clauses, sizeof(int));
  litteral_3   = calloc(n_clauses, sizeof(int));

  // ... printing the header
  printf("c This Formular is generated by permutation \n");
  printf("p cnf %d %d  \n", n_litterals, n_clauses);


  // ... relabelling
  write_relabel(input_filename, sorted, &freq_matrix, &n_clauses, &n_litterals,
		&litteral_1, &litteral_2, &litteral_3);
  // generation of the new sorted cnf file

  /*
 // ... printing the final ordered cnf description
  for (int j = 0; j < n_clauses; j++) {
    printf(" %d %d %d 0 \n",
	   litteral_1[j],
	   litteral_2[j],
	   litteral_3[j]);
  }  
  */
  // freeing memory and exiting
  free(sorted);
  free(litteral_1);
  free(litteral_2);
  free(litteral_3);
  free(freq_matrix);

  
  return EXIT_SUCCESS;

}

