#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/stat.h>

int process_matrix_input(int *matrix);

int main (int argc, char **argv) {
	int mtxshmID;
	int *mtxshmPTR;
	pid_t pid;
	int status;

	// Create shared memory for matrix to be used by all child processes
	if( (mtxshmID = shmget(IPC_PRIVATE, getpagesize(), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR)) == -1 ) {
		perror("shmget error: ");
		exit(1);
	}
	// Attach running process and children to shared memory region
	// Store pointer to the start of shared memory to hold matrix
	mtxshmPTR = (int *) shmat(mtxshmID, NULL, 0);

	int nsize = process_matrix_input(mtxshmPTR);
	
	// Prepare program to be passed	
	char *args[6];
	args[0] = malloc(sizeof(char)*20);
	args[1] = malloc(sizeof(char)*10);
	args[2] = malloc(sizeof(char)*10);
	args[3] = malloc(sizeof(char)*10);
	args[4] = malloc(sizeof(char)*10);
	args[5] = NULL;
	char mtxshmIDstr[40];
	char nsizestr[10];
	char nodestr[10];
	char ndeshmIDstr[40];
	sprintf(mtxshmIDstr, "%d", mtxshmID);
	sprintf(nsizestr, "%d", nsize);
	strcpy(args[0], "./find_paths");
	strcpy(args[1], mtxshmIDstr);
	strcpy(args[2], nsizestr);

	// Create array of n pointers to n child shared  mem regions
	char *ndeshmPTR[nsize];
	int ndeshmID[nsize];

	int node;
	for (node = 0; node < nsize; node++) {
		// Create shared memory that children will use to
		//  send results back to this process
		if((ndeshmID[node] = shmget(IPC_PRIVATE, getpagesize(), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR)) == -1 ) {
			perror("shmget error: ");
			exit(1);
		}
		// Attach running process to region
		ndeshmPTR[node]    = (char *) shmat(ndeshmID[node], NULL, 0);

		// Final set up of program arguments before execd
		sprintf(nodestr, "%d", node);
		strcpy(args[3], nodestr);
		sprintf(ndeshmIDstr, "%d", ndeshmID[node]);
		strcpy(args[4], ndeshmIDstr);
	
		// FORK CHILD PROCESSES AND EXEC FIND_PATHS IN EACH
		pid = fork();
		if (pid < 0) {
			perror("fork error: ");
			exit(1);
		}
		else if (pid==0) {
			if(execvp(args[0],args) == -1) {
				perror("execvp error: ");
				exit(1);
			}
		}
		else {
			;
		}
	}

	// Wait for all child processes to finish before writing results to stdout
	for (node = 0; node < nsize; node++) {
		waitpid(-1, NULL, 0);
	}

	// After child processes finished, read results from shared memory regions
	for (node = 0; node < nsize; node++) {
		printf("%s", ndeshmPTR[node]);
	}

	// Detach shared memory section
	shmdt((void *) mtxshmPTR);

	return 0;
}


/*
  Reads a matrix from stdin (file or terminal)
   and stores values sequentially starting at mtx
  Returns size of columns n in an n*n matrix
   or -1 if error occurs
  To access mtx[i][j], use mtx[i*n+j]
*/
int process_matrix_input(int *mtx) {
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

