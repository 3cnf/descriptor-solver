#ifndef _CNF_SIMPLE_HPP
#define _CNF_SIMPLE_HPP

#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <gmpxx.h>
#include <string>
#include <fstream>
#include <cstdlib>
#include <array>
#include <tuple>
#include <ctime>
#include <boost/multiprecision/gmp.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>

// debug parameter
#define DEBUG 1

// type definitions for matrix, sparse matrix and sparse vector
typedef std::set<boost::multiprecision::mpz_int> s_vector;
typedef std::map<int, s_vector> s_matrix;
typedef std::vector<std::vector<int>> matrix;

// node structure for the Tree structure
struct Node {
  int              idx;   // index (line of CNF matrix)
  int              value; // value (0 or 1)
  std::vector<int> alpha; // vector of alpha_i for the node
  Node            *left;  // left child
  Node            *right; // right child
};

// Tree structure used by compute_all_h_fasy
struct Tree {
  Node *root;                  // root node
  std::string id_cnf;          // the id of the problem (typically the filename)
  
  Tree(std::string an_id);
  ~Tree();
  void destroy_tree(Node *leaf);          
  void create_tree(s_matrix &H, int n_litterals);
  int create_children(Node *cur_node, s_matrix &H, int n_litterals);
};   

// split a string into a 2-uple (id, s_vector)
std::tuple<int, s_vector> split(const std::string &msg, const std::string &separators);

// removing a file
void remove_file(std::string filename);

// create a cnf matrix from an input file
matrix read_input(std::string filename);

// create a sparse vector from an input file
std::tuple<int, s_vector> read_input_vector(std::string filename);

// create a sparse matrix from an input  file
s_matrix read_input_matrix(std::string filename);

// compute the matrics F, G and H for a clause of a cnf matrix  
std::tuple<int, s_matrix, s_matrix, s_matrix> compute_FGH(matrix const &matrix_cnf, int clause);

// given the indexes of 3 litterals, compute the associated index 
boost::multiprecision::mpz_int compute_index3(int index_1, int index_2, int index_3, int n_litterals);

// compute a given row of matrix D telling wich alpha_k are present the row (OK!)
std::vector<int> D_fnct(boost::multiprecision::mpz_int row, int n_litterals);

// add an element to a binary matrix/vector
void add_elem_vector(s_vector &v, boost::multiprecision::mpz_int val);
void add_elem_matrix(s_matrix &matrix, int row, boost::multiprecision::mpz_int col);

// printing sparse matrices and (sparse) vectors
void print_matrix_s(s_matrix const &M);
void print_vector_s(s_vector const &v);
void print_vector(std::vector<int> const &v, unsigned int start = 0);

// saving a sparse matrix/vector in a file
void save_matrix_s(s_matrix const &M, std::string filename);
void save_vector(std::vector<int> const &v, unsigned int start = 0, std::string filename = "sol");

// simplifies a binary matrix/vector
s_matrix simplify_matrix_sparse(s_matrix &matrix_in, int n_litterals);
s_vector simplify_vector_sparse(s_matrix &matrix_in, s_vector &vector_in, int cur_row, int n_litterals);

// create a matrix from a vector
s_matrix generate_matrix(s_vector &v, int row);

// remove a row in the matrix M
void remove_row_in_M(s_matrix &M, int row);

// copy a vector in a given row of M
void copy_vector_in_M(s_matrix &M, s_vector &v, int row);

// copy a given row of M in row
s_vector copy_row_in_v(s_matrix &M, int row);

// check if a sparse vector is empty
bool is_null(s_vector const &v);

// check if 2 vectors are equals component-wise
bool is_equal(std::vector<int> const &v_1, const std::vector<int> &v_2);

// return the number of ones in a given row of a sparse matrix
size_t count_ones_vector(s_matrix &matrix_in, int cur_row);

// compute the vectors I_0 and I_1 from I
std::tuple<s_vector, s_vector> compute_I0_I1(s_vector &I, int n_litterals, int alpha_k);

// from a given binary vector and the number of litterals, return the index in the D matrix
boost::multiprecision::mpz_int compute_index_vect2(std::vector<int> const &v, int n_litterals);

// adding two binary vectors
void add(s_vector &v_1, s_vector &v_2);
s_vector add_out(s_vector const &v_1, s_vector const &v_2);

// component wise binary vector operation (if v_2[1] then out[i] = 1 else out[i] = v_1[i])
std::vector<int> mult(std::vector<int> const &v_1, std::vector<int> const &v_2);

// adding two binary matrices
s_matrix add_matrix(s_matrix const &M_1, s_matrix const &M_2);

// multiply a vector with another one using the matrix D representing the binary vectors
s_vector mult_mat_vec(s_vector &v1, s_vector &v2, int n_litterals);

// return the maximum k of the alpha_i in a vector v
int get_highest_alpha_k(s_vector &v, int n_litterals);

// merges 2 clauses using the matrics H, F and G.
bool merge(s_matrix &H1, s_matrix &H2, s_matrix &F1, s_matrix &F2, s_matrix &G1, 
           s_matrix &G2, std::vector<int> &row_constraint, int id_new_constraint,
           int *lev_rec, int n_litterals);

// given an index and the number of litterals, generate the associated binary vector alpha                      
std::vector<int> generate_alpha(int n_litterals, boost::multiprecision::mpz_int x);

// returns the output of H for a given vector alpha
std::vector<int> h_vector(s_matrix &H, int n_litterals, std::vector<int> &alpha);

// compute the value of a row of H when we replace the alpha_i by some values, i.e. for a given vector alpha
int compute_h_i(std::vector<int> &v_alpha, s_vector &row_H, int n_litterals);

// compute every solution of a matrix H
void compute_all_h(s_matrix &H, int n_litterals);
// efficiently compute every solution of a matrix H using a tree structure
void compute_all_h_fast(s_matrix &H, int n_litterals, std::string filename);

// deterniming a
void merge_matrices(std::string filename);

// mergin a CNF matrix H with a line (vector) of another CNF matrix
void merge_matrix_vector(std::string filename_matrix, std::string filename_vector);

// compute the max litteral in the remaining clauses
std::vector<int> compute_max_lit_remaining_clauses(matrix &input_matrix);

// print the alpha in a row of a matrix
void print_alphas(s_matrix &matrix_in, int cur_row, int n_litterals);

#endif

