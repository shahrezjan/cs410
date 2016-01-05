#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/shm.h>

void find_paths(int *matrix, int nsize, int start, int i, int *visited, char *path, int sum, char *shmresults);
int allvisited(int y, int *vis, int nsize);

int main (int argc, const char *argv[])
{
	const char *arg1 = argv[1];
	const char *arg2 = argv[2];
	const char *arg3 = argv[3];
	const char *arg4 = argv[4];
	
	int *mtxshmPTR;
	int mtxshmID = atoi(arg1);
	int nsize = atoi(arg2);
	int vstart = atoi(arg3);
	char *ndeshmPTR;
	int ndeshmID = atoi(arg4);

	// Re-attach this node's process to shared memory region
	// Store pointer to the start of matrix
	mtxshmPTR = (int *) shmat(mtxshmID, NULL, 0);

	// Re-attach this node's process to results shared mem. reg.
	ndeshmPTR = (char *) shmat(ndeshmID, NULL, 0);

	// Set up buffers before calling recursive fcn
	char path[80];
	path[0] = '\0';
	int visited[nsize];
	int k;
	for(k = 0; k < nsize; k++) { visited[k] = 0; }

	// Recursive fcn to print out valid paths
	find_paths(mtxshmPTR, nsize, vstart, vstart, visited, path, 0, ndeshmPTR);

	return 0;
}

void find_paths(int *matrix, int nsize, int start, int i, int *visited, char *path, int sum, char *shmresults) {
	/* 
	  At any traversed node, this value stores:
	  the running sum + weight of edge traversed
	*/
	int sumUpd;
	int j;
	for (j = 0; j < nsize; j++) {
		if (i!=j && j==start && (matrix[i*nsize+j]) > 0 && allvisited(start, visited, nsize)) {
			sumUpd = sum + matrix[i*nsize+j];
			char oneline[80];
			sprintf(oneline, "%d %s%d %d\n", start, path, j, sumUpd);
			strcat(shmresults, oneline);
		}
		if (i!=j && (matrix[i*nsize+j]) > 0 && visited[j]==0 && j!=start) {
			sumUpd = sum + matrix[i*nsize+j];
			int visited2[nsize];
			char path2[80];
			strcpy(path2, path);
			int k;
			for (k = 0; k < nsize; k++) {
				if (visited[k]==1) visited2[k]=1;
				else visited2[k] = 0;
			}
			visited2[j] = 1;
			char apnd[10]; 
			sprintf(apnd, "%d ", j);		
			strcat(path2,apnd);
			find_paths(matrix, nsize, start, j, visited2, path2, sumUpd, shmresults);
		}
	}

}

// returns 1 if all visited but node y
int allvisited(int y, int *vis, int nsize) {
	int i;
	for (i = 0; i < nsize; i++) {
		if (i!=y && vis[i]==0) return 0;
	}
	return 1;
}
