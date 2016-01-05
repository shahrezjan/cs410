#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <ucontext.h>

static int count = 0;
static int sched = 0;
static const int MAX_NSIZE = 20;
static ucontext_t child[20];
static ucontext_t parent;
static int thread = 0;
static char *stacks[20];
static int graph[4096];
static int nsize;

int process_matrix_input(int *matrix);
void my_thr_create(void (*func) (int), int thr_id);
void find_paths(int start, int i, int *visited, char *path, int sum);
int allvisited(int y, int *vis);
void find_paths_wrapper(int start);

int main (int argc, char **argv)
{
	int i;	// Loop iteration variable
	
	/* 
	  Read matrix from STDIN and store in graph
	*/
	nsize = process_matrix_input(graph);

	/*
	  Create a thread for each starting node by calling my_thr_create
	*/
	for (i = 0; i < nsize; i++) {
		stacks[i] = (char *) malloc(sizeof(char)*SIGSTKSZ);
		my_thr_create(find_paths_wrapper, i);
	}

	/*
	  Begin running first thread
	  Threads will switch round-robin style after each print
	  until all threads are finished
	*/
	swapcontext(&parent, &child[0]);
	
	return 0;
}

//	Create a copy of the current running context,
//	Then config context to run find_paths at the starter node thr_id
void my_thr_create(void (*func) (int), int thr_id) {

	// Copy context of this (parent) process into child then modify
	getcontext(&child[thr_id]);
	child[thr_id].uc_link = &parent;  //set child to return to parent after finished
	child[thr_id].uc_stack.ss_sp = stacks[thr_id];
	child[thr_id].uc_stack.ss_size = SIGSTKSZ;
	child[thr_id].uc_stack.ss_flags = 0;

	makecontext(&child[thr_id], (void (*)(void))func, 1, thr_id);
}

void find_paths_wrapper(int start) {
	// Set up buffers before calling recursive fcn
	char path[80];
	path[0] = '\0';
	int visited[nsize];
	int k;
	for(k = 0; k < nsize; k++) { visited[k] = 0; }

	// Begin graph traversal
	find_paths(start, start, visited, path, 0);
}

void find_paths(int start, int i, int *visited, char *path, int sum) {
	/* 
	  At any traversed node, this value stores:
	  the running sum + weight of edge traversed
	*/
	int sumUpd;
	int j;
	for (j = 0; j < nsize; j++) {
		// Base case - valid path found
		if (i!=j && j==start && (graph[i*nsize+j]) > 0 && allvisited(start, visited)) {
			sumUpd = sum + graph[i*nsize+j];
			fprintf(stdout, "%d %s%d %d\n", start, path, j, sumUpd);
			// Save the current context and switch to next thread!
			swapcontext(&child[start], &child[(start+1)%nsize]);
		}
		// Recursive case
		if (i!=j && (graph[i*nsize+j]) > 0 && visited[j]==0 && j!=start) {
			sumUpd = sum + graph[i*nsize+j];
			int visited2[nsize];
			char path2[80];
			strcpy(path2, path);
			int k;
			for (k = 0; k < nsize; k++) {
				visited2[k] = visited[k];
			}
			visited2[j] = 1;
			char apnd[10]; 
			sprintf(apnd, "%d ", j);		
			strcat(path2,apnd);
			find_paths(start, j, visited2, path2, sumUpd);
		}
	}
}

// returns 1 if all visited but node y
int allvisited(int y, int *vis) {
	int i;
	for (i = 0; i < nsize; i++) {
		if (i!=y && vis[i]==0) return 0;
	}
	return 1;
}

/*
  Reads a matrix from stdin (file or terminal)
   and stores values sequentially starting at mtx
  Returns size of columns n in an n*n matrix
   or -1 if error occurs
  To access mtx[i][j], use mtx[i*n+j]
*/
int process_matrix_input(int *mtx) {
	//

	char *token;
	char *cmdline = NULL;
	ssize_t linebufsize = 0;

	int nsize = 0;
	int pos   = 0;

	// Tokenize first line of matrix input to determine size
	getline(&cmdline, &linebufsize, stdin);
	token = strtok(cmdline, " ");
	while( token != NULL ) {
		mtx[pos] = atoi(token);
		pos++;
		token = strtok(NULL, " ");
	}
	nsize = pos;

	// Read each remaining line of stdin and fill n*n matrix starting with row 1
	while(getline(&cmdline, &linebufsize, stdin) != -1) { 
		if (cmdline[0] == '\n') { break; }
		token = strtok(cmdline, " ");
		while( token != NULL ) {
			mtx[pos] = atoi(token);
			pos++;
			token = strtok(NULL, " ");
		}
	}
	return nsize;
}

