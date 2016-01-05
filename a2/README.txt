Name: Hwa-seung Erstling
BUID: U36770098
CS410 - Assignment 2

In current directory:
	Run "make all" to create executables:
		myshell
		find_paths
		tsp_p
		tsp_t
		tspsort

	Run "make clean" to clean files create from make all and all temporary files


Using 'myshell':
	This is a simple shell.

	$	./myshell
		OR
	$	./myshell < <filename>

	Notes: filename refers to an input file that has shell commands delimited by newlines


Using 'tsp_p':
	This program relies on program 'find_paths' in the same directory.

	$	./tsp_p

	Output: Valid paths and summed weight of all edges traversed 
			Ex: "0 5 2 3 1 4 0 245"

	Notes: Reads a matrix from the standard input.
			Must contain an N*N matrix where columns are 
			delimited by single spaces and rows by newlines.
	Assumptions: Matrix is undirected (i.e. D[i,j] == D[j,i]).

Using 'tsp_t':
	This program creates n-threads for n nodes in the input matrix.
	A very simple scheduler is implemented which switches threads
		after a running thread finds and prints a valid path. 
	The scheduler keeps switching until all threads are finished,
		and demonstrates threads 'pausing' and picking up where
		they left off when switched back to.

	$	./tsp_t

	Output: Same as tsp_p
	Notes: Same matrix input requirements as 'tsp_p'
	Assumptions: Matrix is undirected (i.e. D[i,j] == D[j,i]).
