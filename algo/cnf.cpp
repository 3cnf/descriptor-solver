/*
  CNF-3 SAT - P ?=? NP
  Sparse, GMP and C++11 coding
 
  !!! Requires the GMP and Boost libraries !!!
 
  This code merges a matrix H with a vector s and return the update matrix
 
  Authors: J. Barthelemy and M. Remon
  Version 30 Aug 2017
 
  Compiling:
  > g++ cnf.cpp -o cnf -lgmpxx -lgmp -lm -Wall -O3
  
  If using an old version of gcc, please also add the flag -std=c++11
*/

#include "cnf.hpp"

using namespace std;
using namespace boost::multiprecision;

// Decompose a string according to a separator into a vector of type T.
tuple<int, s_vector> split(const string & msg, const string &separators) {

  set<mpz_int> result;                                           // resulting vector
  int          line_id = 0;                                          // line being currently read;
  mpz_int      token;                                            // one token of the string
  boost::char_separator<char> sep(separators.c_str());           // separator
  boost::tokenizer<boost::char_separator<char> > tok(msg, sep);  // token's generation

  // loop over every token and cast to desired type        
  for (boost::tokenizer<boost::char_separator<char> >::const_iterator i = tok.begin(); i != tok.end(); i++) {
    stringstream s(*i);
    string s_string = s.str();                                // getting the string
    boost::algorithm::trim(s_string);                         // removing trailing blanks
        
    if (i == tok.begin()) {
      line_id = stoi(s_string);                             // casting string to int
    } else {
      token = static_cast<mpz_int>(s_string);               // casting string to mpz_int
      result.insert(token);    
    }
        	    
  }

  // returning the resulting decomposition
  return make_tuple(line_id - 1, result);

}


void remove_file(std::string filename) {

  remove(filename.c_str());
  
}


// reading a sparse matrix from a file
s_matrix read_input_matrix(string filename) {
    
  s_matrix out;
        
 // opening the cnf file
  ifstream f(filename.c_str());
  if (f.is_open() == false) {         
    cout << "Error! Could not open " << filename << "!" << endl;
    exit(EXIT_FAILURE);
  }
    
  string a_line;
    
  // reading the matrix file
  while(getline(f, a_line)) {
        
    int row_id;
    set<mpz_int> row_data;
    tie(row_id, row_data) = split(a_line, " ");        
    out[row_id] = row_data;
        
  }
    
  // closing the file
  f.close();

  // returning the matrix    
  return out;
    
}

// reading a line of a sparse matrix in a file
tuple<int, s_vector> read_input_vector(string filename) {
            
  // opening the cnf file
  ifstream f(filename.c_str());
  if (f.is_open() == false) {         
    cout << "Error! Could not open " << filename << "!" << endl;
    exit(EXIT_FAILURE);
  }
    
  // reading the line
  string a_line;
  getline(f, a_line);
  auto res = split(a_line, " ");
    
  // closing the file
  f.close();
    
  // returnin the result
  return res;
    
}

// reading a cnf matrix from a file
matrix read_input(string filename) {
    
  int n_clauses;
  int n_litterals;       
    
  // opening the cnf file
  ifstream f(filename.c_str());
  if (f.is_open() == false) {         
    cout << "Error! Could not open " << filename << "!" << endl;
    exit(EXIT_FAILURE);
  }
    
  // read the file until we find the dimension of the matrix
  string str;
  do {
    f >> str;
  } while(str.compare("p") != 0);
    
  // extracting the problen description (p cnf n_litterals n_clauses)    
  f >> str >> n_litterals >> n_clauses;
    
  // creating the matrix
  matrix matrix(n_clauses, vector<int>(n_litterals));
    
  // reading data
  int lit_1, lit_2, lit_3;
  int clause = 0;
  while (clause < n_clauses) {
    f >> lit_1 >> lit_2 >> lit_3 >> str;
        
    if (lit_1 > 0) matrix[clause][ lit_1 - 1] =  1;
    else           matrix[clause][-lit_1 - 1] = -1;
    if (lit_2 > 0) matrix[clause][ lit_2 - 1] =  1;
    else           matrix[clause][-lit_2 - 1] = -1;
    if (lit_3 > 0) matrix[clause][ lit_3 - 1] =  1;
    else           matrix[clause][-lit_3 - 1] = -1;
        
    clause++;
  }
    
  // closing the file and returning the matrix
  f.close();      
  return matrix;
    
}

void print_matrix_s(s_matrix const &M) {
    
  for (auto &i:M) {
    cout << "Row " << i.first + 1 << ":";
    for(auto &j:i.second) {
      cout << " " << j;
    }
    cout << endl;
  }    
    
}

void print_vector(vector<int> const &v, unsigned int start) {

  for (unsigned int i = start; i < v.size(); i++) {
    cout << v[i] << " ";
  }
  cout << endl;
    
}

void print_vector_s(s_vector const &v) {
    
  for (auto i:v) {
    cout << i << " ";
  }
  cout << endl;
    
}

void save_matrix_s(s_matrix const &M, string filename) {

  string filename_mat = filename + ".Hmat";
  if (DEBUG > 1) {
    cout << "SAVING MATRIX IN " << filename_mat << endl;
  }
  
  ofstream f(filename_mat.c_str());
  if (f.is_open() == false) {         
    cout << "Error! Could not open " << filename_mat << "!" << endl;
    exit(EXIT_FAILURE);
  }

  for (auto &i:M) {
    // printing the id of the row in front of the data
    f << i.first + 1 << " ";
    for(auto &j:i.second) {
      f << " " << j;
    }
    f << endl;
  }

  f.close();
  
}

void save_vector(vector<int> const &v, unsigned int start, string filename_vec) {

  if (DEBUG > 1) {
    cout << "SAVING IN " << filename_vec << endl;
  }

  // appending the vector at the end of the file
  ofstream f(filename_vec.c_str(), ios::out | ios::app);
  if (f.is_open() == false) {         
    cout << "Error! Could not open " << filename_vec << "!" << endl;
    exit(EXIT_FAILURE);
  }
  
  for (unsigned int i = start; i < v.size(); i++) {
    f << v[i] << " ";
  }
  f << endl;

  f.close();

}

void add_elem_matrix(s_matrix &matrix, int row, mpz_int col) {
    
  // if row not already present in the matrix create it 
  if (matrix.find(row) == matrix.end()) {        
    s_vector new_row;
    new_row.insert(col);
    matrix[row] = new_row;
  }
  // else try to insert the element (if already present, removes it.
  else {        
    auto pair_insert = matrix[row].insert(col);
    if (pair_insert.second == false) { 
      matrix[row].erase(col);
    }    
  }        
    
}

void add_elem_vector(s_vector &v, mpz_int val) {
    
  // try to insert the new element
  auto pair_insert = v.insert(val);
  // check if element was already here, and removes it if it is the cae
  if (pair_insert.second == false) {
    v.erase(val);
  }
    
}

void add(s_vector &v_1, s_vector &v_2) {        
    
  for(auto i:v_2) {
    add_elem_vector(v_1, i);
  }
    
}

s_vector add_out(s_vector const &v_1, s_vector const &v_2) {        
    
  s_vector out;
    
  for (auto i:v_1) {
    add_elem_vector(out, i);
  }
    
  for (auto i:v_2) {
    add_elem_vector(out, i);
  }
    
  return out;
    
}

s_matrix add_matrix(s_matrix const &M_1, s_matrix const &M_2) {
    
  s_matrix out;
    
  for (auto r:M_1) {
    int row = r.first;
    for (auto elem:r.second){
      add_elem_matrix(out, row, elem);
    }
  }
    
  for (auto r:M_2) {
    int row = r.first;
    for (auto elem:r.second){
      add_elem_matrix(out, row, elem);
    }
  }
        
  return out;
    
}

vector<int> mult(vector<int> const &v_1, vector<int> const &v_2) {
    
  vector<int> out(v_1.size(), 0);
    
  for (size_t i = 0; i < v_1.size(); i++) {
    if (v_2[i] == 1) out[i] = 1;
    else             out[i] = v_1[i];            
  }
    
  return out;
    
}

bool is_null(s_vector const &v) {
  if (v.size() == 0) return true;
  else               return false;
}

bool is_equal(vector<int> const &v_1, vector<int> const &v_2) {        
    
  return (v_1 == v_2);    
    
}

vector<int> D_fnct(mpz_int row, int n_litterals) {
    
  vector<int> result(n_litterals + 1, 0);    
  const mpz_int base = 2;
    
  if (row == 0) {
    result[0] = 1;
  } else {
    mpz_int x = row;
    int j = n_litterals;
    while (x > 0) {
      mpz_int res = x % base;
      result[j] = static_cast<int>(res);
      x = x / 2;
      j--;
    }    
  }    
    
  return result;
    
}

mpz_int compute_index3(int index_1, int index_2, int index_3, int n_litterals) {

  const mpz_int base = 2;

  if (index_1 == 0 && index_2 == 0 && index_3 == 0) return 0;
  if (index_1 >  0 && index_2 == 0 && index_3 == 0) return pow(base, n_litterals - index_1);
  if (index_1 >  0 && index_2 >  0 && index_3 == 0) return pow(base, n_litterals - index_1) + pow(base, n_litterals - index_2);
  if (index_1 == 0 && index_2 >  0 && index_3 == 0) return pow(base, n_litterals - index_2);
  if (index_1 == 0 && index_2 == 0 && index_3 >  0) return pow(base, n_litterals - index_3);
  if (index_1 == 0 && index_2 >  0 && index_3 >  0) return pow(base, n_litterals - index_2) + pow(base, n_litterals - index_3);
  if (index_1 >  0 && index_2 == 0 && index_3 >  0) return pow(base, n_litterals - index_1) + pow(base, n_litterals - index_3);
   
  return pow(base, n_litterals - index_1) + pow(base, n_litterals - index_2) + pow(base, n_litterals - index_3);

}

mpz_int compute_index_vect2(vector<int> const &v, int n_litterals) {
    
  const mpz_int base = 2;
    
  mpz_int idx = 0;
  if (v[0] == 1 ) {
    return idx;
  }
    
  for (int i = 1; i < n_litterals + 1; i++) {
    if (v[i] == 1) {
      idx = idx + pow(base, n_litterals - i);
    }
  }
        
  return idx;
    
}

std::tuple<int, s_matrix, s_matrix, s_matrix> compute_FGH(matrix const &matrix_cnf, int clause) {
    
  if (DEBUG >= 2) {
    cout << "DEBUG: IN compute_FGH" << endl;
  }
        
  int n_litterals = matrix_cnf[0].size();      
    
  s_matrix F, G, H;
        
  // loop over the litterals of the clause to retrieve the index
  // NOTE: range of the litteral indices: [1, n_litterals]    
  array<int, 3> index_lit = {0, 0, 0};
  int n_lit = 0;
  for (int k = 0; k < n_litterals; k++) {
    if (matrix_cnf[clause][k] != 0) {
      index_lit[n_lit] = (k + 1) * abs(matrix_cnf[clause][k]) / matrix_cnf[clause][k];
      n_lit++;
    }
  }
    
  // creating the associated rowzs in H
  mpz_int idx;

  // ... litterals 1 and 2 in H    
  int x1 = abs(index_lit[0]);
  idx = compute_index3(x1, 0 , 0, n_litterals);
  add_elem_matrix(H, x1 - 1, idx);

  int x2 = abs(index_lit[1]);
  idx = compute_index3(0, x2, 0, n_litterals);    
  add_elem_matrix(H, x2 - 1, idx);
    
  // ... litteral 3
  int x3 = abs(index_lit[2]);        
    
  if (index_lit[0] > 0 && index_lit[1] > 0 && index_lit[2] > 0) {
    // a1a2a3
    idx = compute_index3(x1, x2, x3, n_litterals);
    add_elem_matrix(H, x3 - 1, idx);    
    // a1a2
    idx = compute_index3(x1, x2, 0, n_litterals);
    add_elem_matrix(H, x3 - 1, idx);
    add_elem_matrix(F, x3 - 1, idx);
    // a1a3
    idx = compute_index3(x1, 0, x3, n_litterals);
    add_elem_matrix(H, x3 - 1, idx);
    // a2a3
    idx = compute_index3(0, x2, x3, n_litterals);
    add_elem_matrix(H, x3 - 1, idx);
    // a1
    idx = compute_index3(x1, 0, 0, n_litterals);
    add_elem_matrix(H, x3 - 1, idx);
    add_elem_matrix(F, x3 - 1, idx);
    // a2
    idx = compute_index3(0, x2, 0, n_litterals);
    add_elem_matrix(H, x3 - 1, idx);
    add_elem_matrix(F, x3 - 1, idx);
    // 1
    idx = compute_index3(0, 0, 0, n_litterals);
    add_elem_matrix(H, x3 - 1, idx);
    add_elem_matrix(F, x3 - 1, idx);
    add_elem_matrix(G, x3 - 1, idx);
  }
  else if (index_lit[0] > 0 && index_lit[1] > 0 && index_lit[2] < 0) {
    // a1a2a3
    idx = compute_index3(x1, x2, x3, n_litterals);
    add_elem_matrix(H, x3 - 1, idx);
    idx = compute_index3(x1, x2, 0, n_litterals);
    add_elem_matrix(G, x3 - 1, idx);
    // a1a3
    idx = compute_index3(x1, 0, x3, n_litterals);
    add_elem_matrix(H, x3 - 1, idx);
    idx = compute_index3(x1, 0, 0, n_litterals);
    add_elem_matrix(G, x3 - 1, idx);
    // a2a3
    idx = compute_index3(0, x2, x3, n_litterals);
    add_elem_matrix(H, x3 - 1, idx);
    idx = compute_index3(0, x2, 0, n_litterals);
    add_elem_matrix(G, x3 - 1, idx);
  }
  else if (index_lit[0] > 0 && index_lit[1] < 0 && index_lit[2] > 0) {                
        
    // a1a2a3
    idx = compute_index3(x1, x2, x3, n_litterals);
    add_elem_matrix(H, x3 - 1, idx);
    // a1a2
    idx = compute_index3(x1, x2, 0, n_litterals);
    add_elem_matrix(H, x3 - 1, idx);
    add_elem_matrix(F, x3 - 1, idx);
    // a2a3
    idx = compute_index3(0, x2, x3, n_litterals);
    add_elem_matrix(H, x3 - 1, idx);
    // a2
    idx = compute_index3(0, x2, 0, n_litterals);
    add_elem_matrix(H, x3 - 1, idx);
    add_elem_matrix(F, x3 - 1, idx);
    // a3
    idx = compute_index3(0, 0, x3, n_litterals);
    add_elem_matrix(H, x3 - 1, idx);
    idx = compute_index3(0, 0, 0, n_litterals);
    add_elem_matrix(G, x3 - 1, idx);
  } 
  else if (index_lit[0] > 0 && index_lit[1] < 0 && index_lit[2] < 0) {
    // a1a2a3
    idx = compute_index3(x1, x2, x3, n_litterals);
    add_elem_matrix(H, x3 - 1, idx);
    idx = compute_index3(x1, x2, 0, n_litterals);
    add_elem_matrix(G, x3 - 1, idx);
    // a2a3
    idx = compute_index3(0, x2, x3, n_litterals);
    add_elem_matrix(H, x3 - 1, idx);
    idx = compute_index3(0, x2, 0, n_litterals);
    add_elem_matrix(G, x3 - 1, idx);
    // a3
    idx = compute_index3(0, 0, x3, n_litterals);
    add_elem_matrix(H, x3 - 1, idx);
    idx = compute_index3(0, 0, 0, n_litterals);
    add_elem_matrix(G, x3 - 1, idx);
  }
  else if (index_lit[0] < 0 && index_lit[1] > 0 && index_lit[2] > 0) {
    // a1a2a3
    idx = compute_index3(x1, x2, x3, n_litterals);
    add_elem_matrix(H, x3 - 1, idx);
    // a1a2
    idx = compute_index3(x1, x2, 0, n_litterals);    
    add_elem_matrix(H, x3 - 1, idx);
    add_elem_matrix(F, x3 - 1, idx);
    // a1a3
    idx = compute_index3(x1, 0, x3, n_litterals);
    add_elem_matrix(H, x3 - 1, idx);
    // a1
    idx = compute_index3(x1, 0, 0, n_litterals);
    add_elem_matrix(H, x3 - 1, idx);
    add_elem_matrix(F, x3 - 1, idx);
    // a3
    idx = compute_index3(0, 0, x3, n_litterals);
    add_elem_matrix(H, x3 - 1, idx);
    idx = compute_index3(0, 0, 0, n_litterals);
    add_elem_matrix(G, x3 - 1, idx);
  }
  else if (index_lit[0] < 0 && index_lit[1] > 0 && index_lit[2] < 0) {
    // a1a2a3
    idx = compute_index3(x1, x2, x3, n_litterals);
    add_elem_matrix(H, x3 - 1, idx);
    idx = compute_index3(x1, x2, 0, n_litterals);
    add_elem_matrix(G, x3 - 1, idx);
    // a1a3
    idx = compute_index3(x1, 0, x3, n_litterals);
    add_elem_matrix(H, x3 - 1, idx);
    idx = compute_index3(x1, 0, 0, n_litterals);
    add_elem_matrix(G, x3 - 1, idx);
    // a3
    idx = compute_index3(0, 0, x3, n_litterals);
    add_elem_matrix(H, x3 - 1, idx);
    idx = compute_index3(0, 0, 0, n_litterals);
    add_elem_matrix(G, x3 - 1, idx);
  }
  else if (index_lit[0] < 0 && index_lit[1] < 0 && index_lit[2] > 0) {
    // a1a2a3
    idx = compute_index3(x1, x2, x3, n_litterals);
    add_elem_matrix(H, x3 - 1, idx);
    // a1a2
    idx = compute_index3(x1, x2, 0, n_litterals);
    add_elem_matrix(H, x3 - 1, idx);
    add_elem_matrix(F, x3 - 1, idx);
    // a3
    idx = compute_index3(0, 0, x3, n_litterals);
    add_elem_matrix(H, x3 - 1, idx);
    idx = compute_index3(0, 0, 0, n_litterals);
    add_elem_matrix(G, x3 - 1, idx);
  }  
  else if (index_lit[0] < 0 && index_lit[1] < 0 && index_lit[2] < 0) {
    // a1a2a3
    idx = compute_index3(x1, x2, x3, n_litterals);
    add_elem_matrix(H, x3 - 1, idx);
    idx = compute_index3(x1, x2, 0, n_litterals);
    add_elem_matrix(G, x3 - 1, idx);
    // a3
    idx = compute_index3(0, 0, x3, n_litterals);
    add_elem_matrix(H, x3 - 1, idx);
    idx = compute_index3(0, 0, 0, n_litterals);
    add_elem_matrix(G, x3 - 1, idx);
  }
            
  return make_tuple(x3 - 1, F, G, H);
        
}

size_t count_ones_vector(s_matrix &matrix_in, int row) {        
    
  // ... check if row in the matrix
  if (matrix_in.find(row) != matrix_in.end()) {
    return matrix_in[row].size();        
  }
  // ... otherwise return 0 
  else {
    return 0;        
  }    
    
}

tuple<s_vector, s_vector> compute_I0_I1(s_vector &I, int n_litterals, int alpha_k) {
    
  // I0: setting the row i in D_IO to 0 of the row in D_I involves alpha_k
  //     recopy the row D_I in D_I0 otherwise
    
  vector<int> d_I(n_litterals + 1, 0);
  vector<int> d_temp(n_litterals + 1, 0);
    
  s_vector I0;
  s_vector I1;
    
  for (auto i:I) {
    d_I = D_fnct(i, n_litterals);
    if (d_I[alpha_k] != 1) {
      add_elem_vector(I0, i);
    }
  }
    
  // I1: column alpha_k is set to 0 in D_i, and add the resulting vector to the corresponding
  //     line in D_I1
    
  for (auto i:I) {
        
    d_I = D_fnct(i, n_litterals);
        
    if (d_I[alpha_k] == 1) {
            
      d_temp = d_I;            
      d_temp[alpha_k] = 0;            
      mpz_int idx = compute_index_vect2(d_temp, n_litterals);                                    
      add_elem_vector(I1, idx);
            
    } else {
      add_elem_vector(I1, i);
    }
        
  }
    
  return make_tuple(I0, I1);
    
}

s_vector mult_mat_vec(s_vector &v1, s_vector &v2, int n_litterals) {
    
  s_vector out;
    
    
  // if one of the vectors are null then there is nothing to do
  if (v1.size() == 0 || v2.size() == 0) {
    return out;
  }    

  mpz_int zero = 0;    
  s_vector d_tmp;
  vector<int> d_tmp_v1(n_litterals + 1, 0);
  vector<int> d_tmp_v2(n_litterals + 1, 0);
  vector<int> v_tmp(n_litterals + 1, 0);
    
  // loop over the components of v2 to break down the multiplication component-wise   
  for (auto comp_v2:v2) {
                
    d_tmp_v2 = D_fnct(comp_v2, n_litterals);
        
    if (comp_v2 > zero) {
            
      // loop over the components of v1
      for (auto comp_v1:v1) {
                            
	if (comp_v1 == zero) {                    
	  add_elem_vector(d_tmp, comp_v2);                    
	} 
	else {
                    
	  d_tmp_v1 = D_fnct(comp_v1, n_litterals);
	  v_tmp = mult(d_tmp_v1, d_tmp_v2);
	  mpz_int idx = compute_index_vect2(v_tmp, n_litterals);                    
	  add_elem_vector(d_tmp, idx);
                    
	}
                
      }
                        
    }
    else {
            
      for (auto comp_v1:v1) {
	add_elem_vector(d_tmp, comp_v1);
      }
            
    }
        
    // add the current results to out and reset d_tmp to 0
    add(out, d_tmp);
    d_tmp.clear();
        
  }
    
  return out;
    
}

s_matrix generate_matrix(s_vector &v, int row) {
    
  s_matrix out;    
  out[row] = v;    
  return out;
    
}

void remove_row_in_M(s_matrix &M, int row) {
    
  // check if row exist    
  auto it = M.find(row);        
    
  // ... if so then remove it
  if (it != M.end() ) {
    M.erase(it);
  }
    
}

void copy_vector_in_M(s_matrix &M, s_vector &v, int row) {
    
  // 1. remove the row in M
  //remove_row_in_M(M, row);
    
  // 2. copy v in M at the given row
  M[row] = v;
    
}

s_vector copy_row_in_v(s_matrix &M, int row) {
    
  s_vector out = M[row];        
  return out;
    
}

s_matrix simplify_matrix_sparse(s_matrix &matrix_in, int n_litterals, int cur_litteral) {

  s_matrix matrix_out;
    
  s_vector v_temp1, v_temp2, v_temp3, v_temp4, v_temp5, v_temp6;
        
  int first_row = -1;
    
  for(int cur_row = 0; cur_row < n_litterals; cur_row++) {
               
    if (count_ones_vector(matrix_in, cur_row) > 0 && cur_row < cur_litteral) {        
      // if (count_ones_vector(matrix_in, cur_row) > 0 ) {        
      // printf("%d : Cur_litteral = %d \n",cur_row,cur_litteral);      
      // v_temp1 <- H_{cur_row}
      v_temp1 = matrix_in[cur_row];           
            
      for (int j = 0; j < cur_row; j++) {
                
	// v_temp2 <- H_{j} (j < cur_row)
	v_temp2 = matrix_in[j];
                
	// v_temp3 <- H_{cur_row} (alpha_j = 0) and v_temp4 <- H_{cur_row} (alpha_j = 1)
	tie(v_temp3, v_temp4) = compute_I0_I1(v_temp1, n_litterals, j + 1);
            
	// v_temp4 <- H_{cur_row}(alpha_j=0) + h_{cur_row}(alpha_j=1)
	add(v_temp4, v_temp3);
                
	// v_temp1 <- [h_{cur_row}(alpha_j=0) + h_{cur_row}(alpha_j=1)]*[h_j]
                
	v_temp1 = mult_mat_vec(v_temp4, v_temp2, n_litterals);
                
	// v_temp5 <- v_temp1 + h_{cur_row}(alpha_j=0)
	v_temp5 = add_out(v_temp1, v_temp3);
	add(v_temp1, v_temp3);                                                                             
                
      }
            
      // copy the new cur_row in the simplified H
      copy_vector_in_M(matrix_out, v_temp5, cur_row);
      
      if (first_row == -1) {
	first_row = cur_row;
      }
            
    } else {
      copy_vector_in_M(matrix_out, v_temp6, cur_row);
    }
            
  }
    
  // copy the first non empty row in matrix_out    
  v_temp6 = matrix_in[first_row];    
  copy_vector_in_M(matrix_out, v_temp6, first_row);
            
  return matrix_out;
    
}

s_vector simplify_vector_sparse(s_matrix &matrix_in, s_vector &vector_in, int cur_row, int n_litterals) {
    
  s_vector out;    
  s_vector v_temp1, v_temp2, v_temp3, v_temp4, v_temp5;    
  mpz_int index = 0;
    
  if (DEBUG >= 2) {
    cout << "DEBUG: simplify_vector_sparse: Vector IN" << endl;
    print_vector_s(vector_in);    
  }
        
  // out <- vector_in
  out = vector_in;
    
  for (int j = 0; j < cur_row; j++) {
              
    if (count_ones_vector(matrix_in, j) > 0) {
        
      // v_temp2 <- H_j (j < cur_row)
      v_temp2 = matrix_in[j];

      // v_temp3 <- H_{cur_row}(alpha_j = 0) and v_temp4 <-  H_{cur_row}(alpha_j = 0)
      tie(v_temp3, v_temp4) = compute_I0_I1(out, n_litterals, j + 1);
            
      // v_temp4 <- H_{cur_row}(alpha_j = 0) + H_{cur_row}(alpha_j = 1)
      add(v_temp4, v_temp3);
            
      // v_temp1 <- [H_{cur_row}(alpha_j = 0) + H_{cur_row}(alpha_j = 1)]*[H_j]
      v_temp1 = mult_mat_vec(v_temp4, v_temp2, n_litterals);
            
      // out <- v_temp1 + H_{cur_row}(alpha_j = 0)
      out = add_out(v_temp1, v_temp3);
            
    }
    else {
            
      // H_j = [0, 0, ..., 0] -> v_temp5 <- H_{cur_row}(alpha_j = 0)
      tie(v_temp5, v_temp4) = compute_I0_I1(out, n_litterals, j + 1);
      out = v_temp5;
            
    }
  }
    
  if (DEBUG >= 2) {
    cout << "DEBUG: simplify_vector_sparse: Vector OUT" << endl;
    print_vector_s( out );    
  }
        
  return out;
    
}

int get_highest_alpha_k(s_vector &v, int n_litterals) {
    
  int max_k = 0;
    
  for (auto i:v) {
        
    vector<int> d_i = D_fnct(i, n_litterals);
        
    for (int j = max_k + 1; j < n_litterals + 1; j++){
      if (d_i[j] == 1) max_k = j;
    }
                
  }
    
  return max_k;
    
}

bool merge(s_matrix &H1, s_matrix &H2, s_matrix &F1, s_matrix &F2, s_matrix &G1, 
           s_matrix &G2, vector<int> &row_constraint, int id_new_constraint,
           int *lev_rec, int n_litterals) {
                  
  int result = true;
    
  // the 2 matrices has differents constraints, i.e; involving differents
  // rows, we then simply need to add the row of H2 (F2, G2) in H1 (F1, G1)
  if (row_constraint[id_new_constraint] == 0) {

    remove_row_in_M(H1, id_new_constraint);
    remove_row_in_M(F1, id_new_constraint);
    remove_row_in_M(G1, id_new_constraint);
        
    H1[id_new_constraint] = H2[id_new_constraint];
    F1[id_new_constraint] = F2[id_new_constraint];
    G1[id_new_constraint] = G2[id_new_constraint];
        
    row_constraint[id_new_constraint] = 1;
                
  } 
  // the new constraint is incompatible with the one already present in H1!
  else {
        
    if (DEBUG >= 2) {
      cout << "INFO: INCOMPATIBILITY for row " << id_new_constraint + 1 << endl;
    }
    

    s_vector out_F1pF2, out_F1xF2;
    s_vector out_G1pG2, out_G1xG2;
    s_vector temp_F, temp_G, temp_H;
    s_vector temp, temp_Z;        
    s_vector I, J, a, ap1;
        
    // F1, F2, G1, G2
    s_vector v_F1 = F1[id_new_constraint];
    s_vector v_F2 = F2[id_new_constraint];
    s_vector v_G1 = G1[id_new_constraint];
    s_vector v_G2 = G2[id_new_constraint];
        
    // (F1 + F2)
    out_F1pF2 = add_out(v_F1, v_F2);
                        
    // (G1 + G2)
    out_G1pG2 = add_out(v_G1, v_G2);
        
    // (F1 * F2)
    out_F1xF2 = mult_mat_vec(v_F1, v_F2, n_litterals);
                                               
    // (G1 * G2)
    out_G1xG2 = mult_mat_vec(v_G1, v_G2, n_litterals);

    if (DEBUG >= 2) {
      cout << "n_litterals = " << n_litterals << endl;                                 
      cout << "F1, F2, F1 + F2, F1xF2:" << endl;
      print_vector_s(v_F1);
      print_vector_s(v_F2);
      print_vector_s(out_F1pF2);
      print_vector_s(out_F1xF2);
      cout << "G1, G2, G1 + G2:" << endl;
      print_vector_s(v_G1);
      print_vector_s(v_G2);
      print_vector_s(out_G1pG2);
      print_vector_s(out_G1xG2);            
    }
                
    // temp_F <- (F1 + F2) * (G1 * G2) + (F1 * F2)   
    // ... temp_F <- (F1 + F2) * (G1 * G2)
    temp_F = mult_mat_vec(out_F1pF2, out_G1xG2, n_litterals);
    // ... temp_F <- (F1 + F2) * (G1 * G2) + (F1 * F2)
    add(temp_F, out_F1xF2);
        
    // temp_G <- (G1 + G2) * (F1 + F2) + (G1 + G2) * (F1 x F2) + (G1 x G2)
    // ... temp_G <- (G1 + G2) * (F1 + F2)
    temp_G = mult_mat_vec(out_G1pG2, out_F1pF2, n_litterals);
    // ... temp <- (G1 + G2) * (F1 x F2)
    temp = mult_mat_vec(out_G1pG2, out_F1xF2, n_litterals);
    // ... temp_G <- (G1 + G2) * (F1 + F2) + (G1 + G2) * (F1 x F2) = temp_G + temp
    add(temp_G, temp);
    // ... temp_G <- (G1 + G2) * (F1 + F2) + (G1 + G2) * (F1 x F2) + (G1 x G2) = temp_G + (G1 x G2)
    add(temp_G, out_G1xG2);
         
    // temp_H <- (a + 1) * F1 + a * G1
    // ... a        
    mpz_int idx = compute_index3(id_new_constraint + 1, 0, 0, n_litterals);
    add_elem_vector(a, idx);        
    // ... ap1
    mpz_int zero = 0;
    add_elem_vector(ap1, zero);
    add_elem_vector(ap1, idx);
    // ... temp_H <- (a + 1) * F1
    temp_H = mult_mat_vec(ap1, temp_F, n_litterals);
    // ... temp <- a * G1
    temp_Z = mult_mat_vec(a, temp_G, n_litterals);
    // ... temp_H <- (a + 1) * F1 + a * G1
    add(temp_H, temp_Z);
         
    // I <- (F1 + F2) * (G1 + G2)
    I = mult_mat_vec(out_F1pF2, out_G1pG2, n_litterals);

    // Modif from Marcel
    copy_vector_in_M(H1, temp_H, id_new_constraint);
    copy_vector_in_M(F1, temp_F, id_new_constraint);
    copy_vector_in_M(G1, temp_G, id_new_constraint);

    // max_row a changer pour que row < id_new_constraint?
    int max_row = get_highest_alpha_k(I, n_litterals);

    // MARCEL : NO SIMPLIFY
    J = simplify_vector_sparse(H1, I, max_row, n_litterals);
    //J = I;

    if (DEBUG >= 2) {
      cout << "INFO: F:" << endl;
      print_vector_s(temp_F);    
      cout << "INFO: G:" << endl;
      print_vector_s(temp_G);        
      cout << "INFO: H:" << endl;
      print_vector_s(temp_H);        
      cout << "INFO: I:" << endl;
      print_vector_s(I);
      cout << "INFO: J:" << endl;
      print_vector_s(J);            
    }
        
    bool feasible = is_null(J);
    // end modif from Marcel
                
    if (feasible == false) {
      if (DEBUG >= 2) {
        cout << "INFO: IMPOSSIBILITY FOUND for row " << id_new_constraint + 1 << "! Using recursivity!" << endl;
      }
      

      // decomposition of I in I_0 and I_1
      // ... find highest k such that alpha_k in I
      int max_k = get_highest_alpha_k(J, n_litterals);
      // ... retrieve index of alpha_k
      mpz_int idx_k = compute_index3(max_k, 0, 0, n_litterals);
      // ... update of I
      s_vector v_k;            
      // Modif from Marcel
      if (count_ones_vector(H1, max_k - 1) > 0) {
	v_k = copy_row_in_v(H1, max_k - 1);
      }
      // end modif from Marcel
      add(J, v_k);            
      // ... generation of I_0 and I_1
      s_vector I0, I1;
      tie(I0, I1) = compute_I0_I1(J, n_litterals, max_k);
            
            
      copy_vector_in_M(H1, temp_H, id_new_constraint);
      copy_vector_in_M(F1, temp_F, id_new_constraint);
      copy_vector_in_M(G1, temp_G, id_new_constraint);
            
      // here, check?
      for (auto r:temp_H) {
	if (r == 1) row_constraint[id_new_constraint] = 1;
      }

      if (is_null(I) == true) {
	max_k = 0;
      }
            
      if (max_k > 0) {
                
	// compute new F2, G2 and H2:
	// * IO dans matrice F2
	// * I1 dans matrice G2
	// * H2 = I
                
	F2 = generate_matrix(I0, max_k - 1);
	G2 = generate_matrix(I1, max_k - 1);
	H2 = generate_matrix(J,  max_k - 1);
                
	remove_row_in_M(H1, max_k - 1);
                
	// recursivity       	
	if (DEBUG >= 2) {
	  cout << "INFO: ... STARTING RECURSIVITY" << endl;
        }
	*lev_rec = *lev_rec + 1;
	result = merge(H1, H2, F1, F2, G1, G2, row_constraint, max_k - 1, lev_rec, n_litterals);
	if (DEBUG >= 2) {
	  cout << "INFO: ... EXITING RECURSIVITY" << endl;
	}
      }
      else {
	result = false;                
      }
                        
    }
        
  }
                
  return result;
              
}

vector<int> generate_alpha(int n_litterals, mpz_int x) {
    
  int size = n_litterals + 1;
  mpz_int base = 2;
    
  vector<int> out(size, 0);
  out[0] = 1;
  for (int i = 1; i < size; i++) {
    mpz_int temp = (x % base);        
    out[i] = static_cast<int>(temp);
        
    x = x / base;
  }
    
  return out;
    
}

vector<int> h_vector(s_matrix &H, int n_litterals, vector<int> &alpha) {
    
  vector<int> out(n_litterals, 0);
    
  int cur_row;
  mpz_int cur_col;         
    
  for (auto &i:H) {
        
    cur_row = i.first;
                                
    for (auto &cur_col:i.second) {
                        
      vector<int> d_i = D_fnct(cur_col, n_litterals);
            
      int prod = 1;
      for (int j = 0; j < n_litterals + 1; j++) {
	if (d_i[j] == 1) {
	  prod = (prod * alpha[j]) % 2; // no need the % 2 ?
	}
      }
            
      out[cur_row] = (out[cur_row] + prod) % 2;
            
    }
        
  }
    
  return out;
    
}

int compute_h_i(vector<int> &v_alpha, s_vector &row_H, int n_litterals) {

  int result = 0;

  // loop over the components of row_H
  for (auto i : row_H) {

    vector<int> list_alpha = D_fnct(i, n_litterals);

    int prod = 1;    
    for (int j = 0; j < n_litterals + 1; j++) {            
      if(list_alpha[j] == 1) {
	prod = prod * v_alpha[j];	
      }           
    }
    
    result = (result + prod) % 2;
    
  }
  
  return result;
  
}

// computing all solution using the matrix H
void compute_all_h(s_matrix &H, int n_litterals) {
          
  int n_sol = 0;
  
  mpz_int base = 2;    
  for (mpz_int j = 0; j < pow(base, n_litterals); j = j + 1) {
        
    vector<int> alpha = generate_alpha(n_litterals, j);
    vector<int> h = h_vector(H, n_litterals, alpha);
        
    vector<int> alpha_without_1(alpha.begin()+1, alpha.end());
        
    if (is_equal(alpha_without_1, h) ==  true) {
      n_sol++;
      cout << "INFO: SOLUTION " << n_sol << " found:";
      print_vector(alpha_without_1); 
    }
        
  }
    
}

// computing the solutions (if any) in an efficient way
void compute_all_h_fast(s_matrix &H, int n_litterals, string filename) {

  // removing the file of solution if already exists
  string filename_sol = filename + ".sol";
  remove_file(filename_sol);

  // printing and saving the matrix H
  print_matrix_s(H);
  save_matrix_s(H, filename);

  // creating a tree with the solutions
  Tree t = Tree(filename_sol);  
  t.create_tree(H, n_litterals);

}

void merge_matrices(string filename) {

  // Read the data
  
  if (DEBUG >= 1) {
    cout << "INFO: INPUT FILE: " << filename << endl;
  }
  vector< vector<int> > input_matrix = read_input(filename);
  int n_clauses = input_matrix.size();
  int n_litterals = input_matrix[0].size();
  if (DEBUG >= 1) {
    cout << "INFO: DIMENSION OF THE PROBLEM: " << endl;
    cout << "-> n_litterals " << n_litterals << endl;
    cout << "-> n_clauses   " << n_clauses << endl;
  }
    // ... printing the data
  if (DEBUG >= 2) {
    for (int i = 0; i < n_clauses; i++) {
      for (int j = 0; j < n_litterals; j++) {            
	cout << " " << setw(2) << input_matrix[i][j];
      }
      cout << endl;
    }
  }

  vector<int> max_xi = compute_max_lit_remaining_clauses(input_matrix);
  if (DEBUG >= 2){
    cout << "MAX XI REMAINING CLAUSES" << endl;
    for(unsigned int i = 0; i < max_xi.size(); i++) {
      cout << "clause " << i << " " << max_xi[i] << endl;
    }
  }
  
  if (DEBUG >= 2) {
    cout << "INFO: GENERATING MATRIX F1, G1 and H1..." << endl;
  }    
  // Generating the initial F, G and H matrices
  vector<int> rows_constraints(n_litterals, 0);       
  int id_new_constraint;
  s_matrix H1, F1, G1, H2, F2, G2, H3;        
    
  tie(id_new_constraint, F1, G1, H1) = compute_FGH(input_matrix, 0);
  rows_constraints[id_new_constraint] = 1;
    
  if (DEBUG >= 2) {
    cout << "DEBUG: id_new_constraint = " << id_new_constraint << endl;
    cout << "DEBUG: First matrix H1" << endl;
    print_matrix_s(H1);
    cout << "DEBUG: First matrix F1 (alpha_k = 0)" << endl;
    print_matrix_s(F1);
    cout << "DEBUG: First matrix G1 (alpha_k = 1)" << endl;
    print_matrix_s(G1);          
  }
    
  // Complete H1 with line alpha_i
  bool flag = false;
  for (int k = 0; k < n_litterals; k++) {
        
    flag = false;
    for ( auto &i:H1 ) {
      int cur_row = i.first;
      if (k == cur_row) {
	flag = true;
      }
    }
        
    if (flag == false) {
      const mpz_int base = 2;
      mpz_int index  = pow(base, n_litterals - (k + 1));
      add_elem_matrix(H1, k, index);
    }
        
  }
         
  if (DEBUG >= 2) {
    cout << "DEBUG: First matrix H1 after completion" << endl;
    print_matrix_s(H1);      
  }
    
  // Loop over the clauses
        
  bool sat = true;
  int cur_lev_rec = 0;
  int max_lev_rec = 0;
  int tot_lev_rec = 0;
    
  for (int c = 1; c < n_clauses; c++) {

    int cur_litteral = 2;        
    // ... computing H2
    if (DEBUG >= 2) {    
      cout << "INFO: GENERATING MATRIX F" << c + 1 << ", G" << c + 1 << " and H" << c + 1 << endl;        
    }
    tie(id_new_constraint, F2, G2, H2) = compute_FGH(input_matrix, c);        
                
    // ... merging with H1 and updating it        
    // ... 1. Simplifiy the matrices H1, F1 and G1
    
    // Marcel : No simplify the row > cur_litteral
    for (int i = 0; i < n_litterals; i++){
      //  printf("Input_matrix[%d][%d] = %d and Cur_litteral = %d \n",c,i,input_matrix[c][i],cur_litteral);      
      if (input_matrix[c][i] !=0 && i+1 > cur_litteral){
	cur_litteral = i+1;
      }
    }
   
    H3 = simplify_matrix_sparse(H1, n_litterals, cur_litteral);                    
    //    H3 = simplify_matrix_sparse(H1, n_litterals);                    
    //H3 = H1;                    

    if (DEBUG >= 2) {
      cout << "DEBUG: Simplified Matrix H1" << endl;
      print_matrix_s(H1);            
    }
        
    F1.clear();
    G1.clear();

    s_vector v_temp1, v_temp2, v_temp3;    
    for (int cur_row = 0; cur_row < n_litterals; cur_row++) {

      v_temp1.clear();
      
      if (count_ones_vector(H1, cur_row) > 0) {
	// v_temp1 <- H_{cur_row}                
	v_temp1 = H1[cur_row];                
      }
            
      tie(v_temp2, v_temp3) = compute_I0_I1(v_temp1, n_litterals, cur_row + 1);
      copy_vector_in_M(F1, v_temp2, cur_row);
      copy_vector_in_M(G1, v_temp3, cur_row);
            
    }

    if (DEBUG >= 2) {
      cout << "DEBUG: Simplified Matrix F1" << endl;
      print_matrix_s(F1);
      cout << "DEBUG: Simplified Matrix G1" << endl;
      print_matrix_s(G1);            
    }
        
    // ... 2. Simplifiy the matrices H2, F2 and G2 
    v_temp1 = copy_row_in_v(H2, id_new_constraint);

    // Marcel : No simplify
    v_temp2 = simplify_vector_sparse(H1, v_temp1, id_new_constraint, n_litterals);        
    //v_temp2 = v_temp1;
    
    copy_vector_in_M(H3, v_temp2, id_new_constraint);
        
    if (DEBUG >= 2) {
      cout << "INFO: Simplified matrix H2:" << endl;
      print_matrix_s(H3);            
    }

    // H2 <- H3
    H2 = H3;
        
    F2.clear();
    G2.clear();
        
    for (int cur_row = 0; cur_row < n_litterals; cur_row ++) {

      v_temp1.clear();
      
      if (count_ones_vector(H2, cur_row) > 0 ) {           
	// v_temp1 <- H_{cur_row}
	v_temp1 = H2[cur_row];
      }
        
      tie(v_temp2, v_temp3) = compute_I0_I1(v_temp1, n_litterals, cur_row + 1);
      copy_vector_in_M(F2, v_temp2, cur_row);
      copy_vector_in_M(G2, v_temp3, cur_row);
                        
    }                             

    if (DEBUG >= 2) {
      cout << "DEBUG: Simplified Matrix F2" << endl;
      print_matrix_s(F2);
      cout << "DEBUG: Simplified Matrix G2" << endl;
      print_matrix_s(G2);            
    }       
    
    // ... 3. Merging the simplified matrices
        
    cur_lev_rec = 0;
    sat = merge(H1, H2, F1, F2, G1, G2, rows_constraints, id_new_constraint, &cur_lev_rec, n_litterals);
    
    if (DEBUG >= 2) {
      cout << "INFO: ... levels of recursivity required: " << cur_lev_rec << endl;
    }
    if (max_lev_rec < cur_lev_rec) {
      max_lev_rec = cur_lev_rec;
    }        
    tot_lev_rec = tot_lev_rec + cur_lev_rec;
        
    if (DEBUG >= 2) {
      cout << "INFO: After merge - New matrix H1:" << endl;
      print_matrix_s(H1);
      cout << "INFO: After merge - New matrix F1:" << endl;
      print_matrix_s(F1);
      cout << "INFO: After merge - New matrix G1:" << endl;
      print_matrix_s(G1);
    }

    if (DEBUG >= 1) {
      cout << "INFO: rows constraints:" << endl;
    }
    size_t max_h = 0;
    int max_k = 0;
    for (int l = 0; l < n_litterals; l++) {
      size_t n_ones = count_ones_vector(H1, l);
      if (DEBUG >= 1) {
	cout << c + 1 << " : Litteral " << l + 1 << " :  " << n_ones;
	cout << " | alphas : ";
	print_alphas(H1, l, n_litterals);
      }
      if ( n_ones > max_h ) {
	max_h = n_ones;
	max_k = l + 1;
      } 
    }

    if (DEBUG >= 2) {
      cout << c +1 << " : Maximum at litteral " << max_k << " : " << max_h << endl;
    }
        
    if (sat == false) break;
        
  }

  if (DEBUG >= 1) {
    cout << "INFO: MAX LEVEL OF RECURSIVITY:   " << max_lev_rec << endl;
    cout << "INFO: TOTALS CALLS OF RECURSIVITY:" << tot_lev_rec << endl;
  }
  
  // Printing the solutions if any    
  if (sat == true) {
    cout << "INFO: SOLUTION(S) FOUND!" << endl;
    if (DEBUG >= 2){
      compute_all_h(H1, n_litterals);
    }
    compute_all_h_fast(H1, n_litterals, filename);
  } else {
    cout << "INFO: NO SOLUTION FOUND!" << endl;
  }  

}

void merge_matrix_vector(string filename_matrix, string filename_vector, int n_litterals) {

  // 0. Read the data

  s_matrix H1 = read_input_matrix(filename_matrix);
  s_vector v;
  int row;
  tie(row, v) = read_input_vector(filename_vector);  

  vector<int> row_constraints(n_litterals);
  
  if (DEBUG >= 1) {
    cout << "INFO: rows_constraints:" << endl;
  }
  size_t max_h = 0;
  int max_k = 0;
  for (int l = 0; l < n_litterals; l++) {
    size_t n_ones = count_ones_vector(H1, l);
    if (DEBUG >= 1) {
      cout << "Litteral " << l + 1 << " :  " << n_ones << endl;
    }
    if ( n_ones > max_h ) {
      max_h = n_ones;
      max_k = l + 1;
    }
    row_constraints[l] = n_ones;
    // Marcel
    if (n_ones == 1) {
      row_constraints[l] = 0;
    }
    // End Marcel
  }
     
  if (DEBUG >= 1) {
    cout << "INFO: Maximum at litteral " << max_k << " : " << max_h << endl;
  }
    
  // 1. Simplify matrix H to generate H1, F1 and G1
    
  // Marcel : no simplify
  s_matrix H3 = simplify_matrix_sparse(H1, n_litterals, n_litterals);
  H1 = H3;
  //s_matrix H3 = H1;  // before: comment both line above
    
  if (DEBUG >= 1) {
    cout << "DEBUG: Simplified Matrix H:" << endl;
    print_matrix_s(H1);
  }  

  s_matrix F1, G1;
  s_vector v_temp1, v_temp2, v_temp3;
  
  for (int cur_row = 0; cur_row < n_litterals; cur_row++) {

    v_temp1.clear();
    
    if (count_ones_vector(H1, cur_row) > 0) {
      // v_temp1 <- H_{cur_row}                
      v_temp1 = H1[cur_row];                
    }
            
    tie(v_temp2, v_temp3) = compute_I0_I1(v_temp1, n_litterals, cur_row + 1);
    copy_vector_in_M(F1, v_temp2, cur_row);
    copy_vector_in_M(G1, v_temp3, cur_row);
    
  }

  // 2. Vector v is put in H2[row]
  
  s_matrix H2, F2, G2;
  H2[row] = v;

  // 3. Simplify matrices H2, F2 and G2
  
  
  v_temp1 = copy_row_in_v(H2, row);

  // Marcel : no simplify
  v_temp2 = simplify_vector_sparse(H1, v_temp1, row, n_litterals);
  //v_temp2 = v_temp1;
              
  copy_vector_in_M(H3, v_temp2, row);
        
  if (DEBUG >= 1) {
    cout << "INFO: Simplified matrix H2:" << endl;
    print_matrix_s(H3);            
  }

  // H2 <- H3
  H2 = H3;
          
  for (int cur_row = 0; cur_row < n_litterals; cur_row ++) {

    v_temp1.clear();
    
    if (count_ones_vector(H2, cur_row) > 0 ) {           
      // v_temp1 <- H_{cur_row}
      v_temp1 = H2[cur_row];
    }
        
    tie(v_temp2, v_temp3) = compute_I0_I1(v_temp1, n_litterals, cur_row + 1);
    copy_vector_in_M(F2, v_temp2, cur_row);
    copy_vector_in_M(G2, v_temp3, cur_row);
                        
  }

  // 4. Merging the simplified matrices
        
  int cur_lev_rec = 0;
  bool sat = merge(H1, H2, F1, F2, G1, G2, row_constraints, row, &cur_lev_rec, n_litterals);        
  if (DEBUG >= 1) {
    cout << "INFO: ... levels of recursivity required: " << cur_lev_rec << endl;
  }
  if (DEBUG >= 1) {
    if (sat == false) {
      cout << "INFO: COULD NOT MERGE! NO SOLUTION FOUND!" << endl;
    } else {
      cout << "INFO: New matrix H1:" << endl;
      print_matrix_s(H1);

      cout << "INFO: New matrix F1:" << endl;
      print_matrix_s(F1);

      cout << "INFO: New matrix G1:" << endl;
      print_matrix_s(G1);
    
    }
  }
          
  cout << "INFO: new rows_constraints:" << endl;
  max_h = 0;
  max_k = 0;
  for (int l = 0; l < n_litterals; l++) {
    size_t n_ones = count_ones_vector(H1, l);
    if (DEBUG >= 1) {
      cout << "Litteral " << l + 1 << " :  " << n_ones << endl;
    }
    if ( n_ones > max_h ) {
      max_h = n_ones;
      max_k = l + 1;
    }
  }
  if(DEBUG >= 1) {
    cout << "INFO: Maximum at litteral " << max_k << " : " << max_h << endl;
  }
   
  // Printing the solutions if any
    
  if (sat == true) {
    cout << "INFO: SOLUTION(S) FOUND!" << endl;
    if (DEBUG >= 2){
      compute_all_h(H1, n_litterals);
    }
    compute_all_h_fast(H1, n_litterals, filename_matrix);
  } else {
    cout << "INFO: NO SOLUTION FOUND!" << endl;
  }  
  
}

vector<int> compute_max_lit_remaining_clauses(matrix &input_matrix) {

  int n_clauses = input_matrix.size();
  int n_lit     = input_matrix[0].size();
  int max_lit   = 0;
  vector<int> res(n_clauses);
  
  for (int i = n_clauses - 1; i >= 0; i--) {

    int max_cur_clause = 0;
    for (int k = 0; k < n_lit; k++ ) {
      if (input_matrix[i][k] != 0) {
	max_cur_clause = k + 1;
      }
    }
        
    if (max_lit < max_cur_clause) {
      max_lit = max_cur_clause;
    }
    res[i] = max_lit;
    
  }

  return res;
  
}

void print_alphas(s_matrix &matrix_in, int row, int n_litterals) {

  // if the row of the matrix is empty, nothing to print
  if (matrix_in.find(row) == matrix_in.end()) {
    cout << endl;
  }
  // ... else print the alpha is
  else {

    // loop over the elements of the row
    for (auto k : matrix_in[row]) {

      cout << " ";
      
      // transform it in the product of alpha_i using the dictionnary
      vector<int> list_alpha = D_fnct(k, n_litterals);

      if ( list_alpha[0] == 1 ) cout << "1";
      
      for( int i = 1; i < n_litterals + 1; i++) {

	if(list_alpha[i] == 1) {
	  cout << "a" << i;
	}
	
      }
      
      
    }

    cout << endl;
    
  }
  
}

Tree::Tree(string an_id) {
  root = new Node;
  id_cnf = an_id;
}

Tree::~Tree() {
  cout << "Destroying the tree!" << endl;
  destroy_tree(root);
}

void Tree::destroy_tree(Node *leaf) {
  if(leaf!=NULL) {
    destroy_tree(leaf->left);
    destroy_tree(leaf->right);    
    delete leaf;
  }
}

void Tree::create_tree(s_matrix &H, int n_litterals) {

  cout << "Building a tree from a matrix H..." << endl;

  // creating root node
  vector<int> cur_sol(n_litterals + 1, 0);
  cur_sol[0] = 1;
  root->alpha = cur_sol;
  root->value = 42;
  root->idx   = 0;
  root->left  = NULL;
  root->right = NULL;

  // creating the children
  int n_sol = create_children(root, H, n_litterals);
    
  cout << "Done! Found " << n_sol << " solutions" << endl;
  
}

// starting from one node, build the children
int Tree::create_children(Node *parent_node, s_matrix &H, int n_litterals) {

  //cout << "in create_children" << endl;

  int n_leaves = 0;
  
  // compute child idx = associated row of matrix H
  int child_idx = parent_node->idx + 1;

  // determine vector of alpha: right is same as parent, left is obtained by setting 1 at position child_idx
  vector<int> v_alpha_left  = parent_node->alpha;
  vector<int> v_alpha_right = parent_node->alpha;
  v_alpha_right[child_idx]  = 1;
  
  // compute values of H_i for left and right childs
  int h_i_left  = compute_h_i(v_alpha_left,  H[child_idx - 1], n_litterals);
  int h_i_right = compute_h_i(v_alpha_right, H[child_idx - 1], n_litterals);

  // creating the children nodes
  if (h_i_left != h_i_right) {
    // if both values are different, create 2 children    
    
    Node *l_child = new Node;
    l_child->idx = child_idx;
    l_child->value = h_i_left;
    l_child->alpha = v_alpha_left;
    l_child->left = NULL;
    l_child->right = NULL;
    parent_node->left = l_child;
    
    Node *r_child = new Node;
    r_child->idx = child_idx;
    r_child->value = h_i_right;
    r_child->alpha = v_alpha_right;
    r_child->left = NULL;
    r_child->right = NULL;
    parent_node->right = r_child;

    // creating children
    if (child_idx < n_litterals) {
      n_leaves = n_leaves + create_children(l_child, H, n_litterals);
      n_leaves = n_leaves + create_children(r_child, H, n_litterals);      
    } else {
      // bottom of the tree: printing the solutions
      print_vector(v_alpha_left, 1);
      print_vector(v_alpha_right, 1);
      save_vector(v_alpha_left, 1, this->id_cnf);
      save_vector(v_alpha_right, 1, this->id_cnf);
      
      // return the number of solutions
      return 2;
    }
    
  } else if (h_i_left == 0) {
    // if values == 0, create left child only
    //cout << "going left only" << endl;
    Node *l_child = new Node;
    l_child->idx = child_idx;
    l_child->value = h_i_left;
    l_child->alpha = v_alpha_left;
    l_child->left = NULL;
    l_child->right = NULL;
    parent_node->left = l_child;

    // check if we reach the bottom
    if (child_idx < n_litterals) {
      n_leaves = n_leaves + create_children(l_child, H, n_litterals);
    } else {
      // bottom of the tree: printing the solutions
      print_vector(v_alpha_left, 1);
      save_vector(v_alpha_left, 1, this->id_cnf);

      // return the number of solutions
      return 1;
    }
    
  } else {
    // if values == 1, create right child only
    //cout << "going right only" << endl;
    Node *r_child = new Node;
    r_child->idx = child_idx;
    r_child->value = h_i_right;
    r_child->alpha = v_alpha_right;
    r_child->left = NULL;
    r_child->right = NULL;
    parent_node->right = r_child;

    // check if we reached the bottom
    if (child_idx < n_litterals) {
      n_leaves = n_leaves + create_children(r_child, H, n_litterals);
    } else {
      // bottom of the tree: printing the solutions
      print_vector(v_alpha_right, 1);
      save_vector(v_alpha_right, 1, this->id_cnf);

      // return the number of solutions
      return 1;
    }

  }
  
  return n_leaves;
  
}

int main(int argc, char* argv[]) {    

  // Starting the stopwatch    
  clock_t start_t, end_t;
  start_t = clock();

  // Solving the CNF problem if a cnf file is provided
  if (argc == 2 ) {
    // ... reading the filename of the file containing the cnf problem
    string filename(argv[1]);
    // ... solving the problem
    merge_matrices(filename);
  }
  // Merging a matrix with a vector
  else if (argc == 4) {
    string filename_matrix(argv[1]);
    string filename_vector(argv[2]);
    int    n_litterals(stoi(argv[3]));
    merge_matrix_vector(filename_matrix, filename_vector, n_litterals);
  }
  else {
    cout << "ERROR: either provide" << endl;
    cout << " - a path to a cnf file;" << endl;
    cout << " - or a path to a file containing a sparse matrix, a path to a file containing a sparse vector and the number of litterals!" << endl;
  }
    
  // Stopping the stopwatch    
  end_t = clock();
  double cpu_time = ((double) end_t - start_t) / CLOCKS_PER_SEC;
  if (DEBUG >= 1) {
    cout << "INFO: DONE! Time required: " << cpu_time << endl; 
  }
  
  return EXIT_SUCCESS;
    
}
