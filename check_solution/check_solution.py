#!/usr/bin/python3

import re
import sys

def read_cnf(filename):
    '''Read a cnf file

    Attention: The file has to be sorted via the permutation program

    Keyword arguments:
    
    filename -- the file containg a cnf problem description.
    
    Returns: a t-uple containing a numpy array and an integer
             giving the number of variables in the cnf problem.
    '''
    
    print('Reading a sorted CNF input file...')

    n_vars     = 0
    n_clauses  = 0
    cnf_matrix = list()
    cur_line   = 0
    
    with open(filename) as in_file:
        for a_line in in_file:            
            
            tokens = a_line.split()            

            if len(tokens) > 0:            
                # problem description
                if tokens[0] == 'p':
                    n_vars     = int(tokens[2])
                    n_clauses  = int(tokens[3])
                
                # filling the cnf matrix
                if re.search('^(-|[1-9])', tokens[0] ) != None:
                    cnf_matrix.append([int(x) for x in tokens[:-1]])
                    cur_line = cur_line + 1

    print('-> Info:')            
    print('     filename : {0}'.format(filename))
    print('     variables: {0}'.format(n_vars))
    print('     clauses  : {0}'.format(n_clauses))

    return (cnf_matrix, n_vars, n_clauses)


def read_solutions(filename):
    '''Read the solutions from a file

    Keyword arguments:

    filename -- the file containing the set of solution to test    

    Returns: the list of solutions to test.
    '''

    solutions = list()
    
    with open(filename) as in_file:
        for a_line in in_file:
            cur_sol = a_line.split()
            solutions.append([bool(int(x)) for x in cur_sol])
    
    return solutions

def check_solutions(cnf_matrix, list_solutions):
    '''Check the solution of the CNF problem
    '''

    n_sol = 0

    # testing each solution
    for sol in list_solutions:

        #print("Testing solution {}".format([int(x) for x in sol]))
        test_sol = True

        # testing each clause with the current solution
        for clause in cnf_matrix:            
            x = list()

            for i in range(3):
                if clause[i] > 0:
                    x.append(sol[clause[i] - 1])
                else:
                    x.append(not sol[abs(clause[i]) - 1])
                    
            #print("-> {}".format(x))

            test_sol = test_sol and (x[0] or x[1] or x[2])
            #print("--> {}".format(test_sol))
            if not test_sol:
                break

        if test_sol:
            print("SOL {} ok!".format([int(x) for x in sol]))
            n_sol = n_sol + 1                
    
    return n_sol


if __name__ == '__main__':

    file_cnf = sys.argv[1]
    file_sol = sys.argv[2]
    
    cnf_matrix, n_vars, n_sol = read_cnf(file_cnf)
    cnf_solutions = read_solutions(file_sol)

    #print(cnf_matrix)
    #print(cnf_solutions)
    
    n_sol = check_solutions(cnf_matrix, cnf_solutions)
    print("Found {} solution!".format(n_sol))
        


