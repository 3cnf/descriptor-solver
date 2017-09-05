#!/usr/bin/python3

import numpy as np
import re
import networkx as nx
import sys

def read_cnf(filename):
    '''Read a cnf file.

    Keyword arguments:
    
    filename -- the file containg a cnf problem description
    
    Returns: a t-uple containing a numpy array and an integer
             giving the number of variables in the cnf problem

    Attention: The file has to be sorted via the permutation program
    '''
    
    n_vars     = 0
    n_clauses  = 0
    cnf_matrix = None
    cur_line   = 0
    
    with open(filename) as in_file:
        for a_line in in_file:
            tokens = a_line.split()            

            #print(a_line.rstrip())

            # problem description
            if tokens[0] == 'p':
                n_vars     = int(tokens[2])
                n_clauses  = int(tokens[3])
                cnf_matrix = np.empty(shape=(n_clauses, 3), dtype=int)

            # filling the cnf matrix
            if re.search('^(-|[1-9])', tokens[0] ) != None:            
                #print('{0}'.format(a_line.rstrip()))
                cnf_matrix[cur_line] = np.array([tokens[0], tokens[1], tokens[2]])
                cur_line = cur_line + 1

    print('-> Info:')            
    print('     filename : {0}'.format(filename))
    print('     variables: {0}'.format(n_vars))
    print('     clauses  : {0}'.format(n_clauses))

    return (cnf_matrix, n_vars)


def sort_cnf(matrix):
    '''Sort the elements of each row in matrix by ascending absolute value.

    Keyword arguments:
    
    matrix -- the matrix to be sorted
    
    Returns: a numpy array    
    '''

    sorted_idx    = np.argsort(np.abs(matrix),axis=1)
    sorted_matrix = np.empty(shape=matrix.shape, dtype=int)
    
    for idx in range(matrix.shape[0]):
        sorted_matrix[idx] = np.array(matrix[idx])[sorted_idx[idx]]
            
    return sorted_matrix


def compute_V(filename):
    ''' Compute the sets V(x_t) for each variable x_t for a cnf problem defined in a file.
    
    V(x_t) is the set of every variable connected with the variable x_t in the
    cnf problem. 
    
    Keyword elements:
    
    filename -- the file containing the definition of the CNF problem

    Returns: a dictionnary V whose key are the litterals x_t and values are the sets V(x_t)
    '''
    
    y = list()
    x = list()

    (cnf_mat, n_vars) = read_cnf(filename)
    cnf_mat = sort_cnf(cnf_mat)
    
    G = nx.Graph()
    Nodes = set()
    for row in cnf_mat:
        G.add_nodes_from({abs(row[0]), abs(row[1]), abs(row[2])})
        G.add_edges_from([(abs(row[0]), abs(row[2])), (abs(row[1]), abs(row[2]))])
        Nodes.add(row[2])
       
    # Suppressing the first trivial clauses

    flag = 1
    for lit in range(n_vars, 1, - 1):
        if (((lit not in Nodes) or (-lit not in Nodes)) and (flag == 1)):
            if (lit in G.nodes()):
                G.remove_node(lit)
        else:
            flag = 0
               
    # Getting V from the graph

    V = dict()
    for lit in G.nodes():
        V[lit]={lit}

    # computing V(x_l) adding the variables occuring in V(x_k) ^ V(-x_k) with k > l

    V_temp2 = set()
    V_cumul = set()
    V_cumul_final = set()
    Phi = set()
    prev_row = 0
    prev_max_v = max(G.nodes())

    # Algorithm to compute #V(x_j)
    for row in cnf_mat:
        if abs(row[2]) in G.nodes():
            # We only consider variable appearing as positive and negative litteral
            if (row[2] in Nodes) and (-row[2] in Nodes):
                #print('row[2] = {}'.format(row[2]))
                # We compute #V(x) for clauses whose higher variable
                # is first in a negative mode (after a positive mode
                # for the previous variable (the clause are sorted)
                if (prev_row > 0 and row[2] < 0):
                    Phi.clear()
                    # We compute the updating part of the algorithm for the previous variable
                    # As the clauses are sorted, this variable will not appear any more in the algorithm
                    # We can compute #V as the final result for this variable
                    for k2 in V[abs(prev_row)]:
                        # Phi= Phi | V[k2] | {k2}
                        Phi= Phi | {k2}
                    V[abs(prev_row)] = Phi|{abs(prev_row)}
                    #print('Final : Phi[{}] = {}'.format(abs(prev_row),sorted(V[abs(prev_row)])))
                    print('#W[{}] = {}'.format(abs(prev_row),len(V[abs(prev_row)])))
                    
                    y.append(len(V[abs(prev_row)]))
                    x.append(abs(prev_row))
                    V_temp2.clear()
                    V_cumul_final = V_cumul
                    
                    # We compute V(x) for the clauses whose higher variable
                    # are in negative mode
                if (row[2] < 0):
                    # We put all litterals in a cumulative set 
                    V_cumul = V_cumul |{row[0],row[1],row[2]}
                        
                    # we put the litterals of the clause in V(x_max)
                    V[abs(row[2])].update({abs(row[0]),abs(row[1])})
                    V_temp2.update(V[abs(row[2])])
                    V_temp2.update({abs(row[0]),abs(row[1])})
                    if (abs(row[2]) in V_temp2):
                        V_temp2.remove(abs(row[2]))
                    #print('V[{}] = {}'.format(abs(row[2]),sorted(V[abs(row[2])])))
                    #print('V_temp2 = {}'.format(sorted(V_temp2)))
                    prev_row=row[2]

                # When the variable x_max is in negative mode (after having appeared
                # in positive mode), impossibilities occur and each new clause
                # generates a new constraint for the second maximal variable, or third
                # fourth, ... if the previous maximal variable already "got" a previous constraint
                if row[2] > 0:
                    V_cumul = V_cumul |{row[0],row[1],row[2]}
                    # print('V_cumul = {}'.format(V_cumul))

                    V[abs(row[2])].update({abs(row[0]),abs(row[1])})
                    #print('V[{}] = {}'.format(abs(row[2]),sorted(V[abs(row[2])])))
                    #print('V_temp2 = {}'.format(sorted(V_temp2)))
                    # the generated constraint concerns the second maximal variable  : max_v
                    # V(max_v) = the set of the variables occuring in the clauses with
                    # x_max in a positive mode, plus the variable from the current clause 
                    max_v = max(V_temp2|{abs(row[0]),abs(row[1])})
                    #print('max_v = {}'.format(max_v))
                    #print('V[{}] = {}'.format(max_v,sorted(V[max_v])))
                    # max_v was already constrainted by an impossibility if V(max_x) != 0  
                    if (V[max_v] != {max_v}):
                        V[max_v] = V[max_v] | V_temp2 | {abs(row[0]),abs(row[1])}
                        V[max_v].discard(max_v)
                        #print('V[{}] = {}'.format(max_v,sorted(V[max_v])))
                        prev_row = row[2]
                        max_v_2 = max(V[max_v])
                        V[max_v_2] = V[max_v_2] | V[max_v]
                        V[max_v_2].discard(max_v)
                        #print('max_v_2 = {}'.format(max_v_2))
                        #print('V[{}] = {}'.format(max_v_2,sorted(V[max_v_2])))
                        while (V[max_v_2] != {max_v_2} and max_v_2 > 2):
                            V[max_v_2] = V[max_v_2] | V[max_v]
                            for k in range(max_v_2,max_v) :
                                V[max_v_2].discard(k)
                            max_v_2 = max(V[max_v_2])
                            #print('max_v_k = {}'.format(max_v_2))
                            #print('V[{}] = {}'.format(max_v_2,sorted(V[max_v_2])))

                        V[max_v_2] = V[max_v_2] | V[max_v]
                        for k in range(max_v_2,max_v) :
                            V[max_v_2].discard(k)
                        #print('max_v_final = {}'.format(max_v_2))
                        #print('V[{}] = {}'.format(max_v_2,sorted(V[max_v_2])))

                    if (V[max_v] == {max_v}) : 
                        V[max_v] = V[max_v] | V_temp2 | {abs(row[0]),abs(row[1])}
                        V[max_v].discard(max_v)
                        #print('V[{}] = {}'.format(max_v,sorted(V[max_v])))
                        prev_row = row[2]

                    
    for row in cnf_mat:
        if (abs(row[2]) in G.nodes()):
            V[abs(row[2])].add(abs(row[0]))
            V[abs(row[2])].add(abs(row[1]))

            
    max_phi = 0
    max_k   = 0    
    for k in range(1, len(x)) :
        if max_phi < y[k]:
            max_phi = y[k]
            max_k = x[k]

    print('Max #W = {} for W[ {} ]'.format(max_phi + 1, max_k))

    return V
        
if __name__ == '__main__':

    file_in = sys.argv[1]
    compute_V(file_in)

    
    
    
