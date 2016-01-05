#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

static int printed[4096];

int alreadyprinted(int i, int numlines) {
	int z;
	for (z = 1; z < numlines; z++) {
		if (printed[z] == i) { return 1; }
	}
	return 0;
}

int main (int argc, char **argv) {

	char linetemp1[100];
	char linetemp2[100];
	char *token1;
	char *token2;
	char *cmdline = NULL;
	ssize_t linebufsize = 0;

	int distance[500];
	char *line[500];
	int numlines = 0;
	int i, j;

	// Read each line; fill line array and distance array
	while(getline(&cmdline, &linebufsize, stdin) != -1) {
		if(cmdline[0] != EOF && cmdline[0] != '\n') {
			char temp[100];
			strcpy(temp, cmdline);
			numlines++;
			line[numlines] = cmdline;
			cmdline = NULL;
			linebufsize = 0;
			token1 = strtok(temp, " \n");
			while (token1 != NULL) {
				if ( (token2 = strtok(NULL, " \n")) == NULL ) {
					distance[numlines] = atoi(token1);
					token1 = token2;
					break;
				}
				token1 = token2;
			}
		} 
	}

	// Output line with smallest distance that hasn't already been outputted
	int count = 0;
	int min = INT_MAX;
	int idx = -1;
	while (count < numlines+1) {
		for (i = 1; i < numlines+1; i++) {
			if (distance[i] < min && !alreadyprinted(i, numlines)) {
				min = distance[i];
				idx = i;
			}
		}
		printed[count] = idx;
		count++;
		min = INT_MAX;
		idx = -1;
	}

	for (i = 1; i < numlines+1; i++) {
		printf("%s", line[printed[i]]);
	}

	return 0;

	// Sort distance array
	int t;
	for (j=0 ; j<(numlines-1) ; j++) {
		for (i=0 ; i<(numlines-1) ; i++) {
			if (distance[i+1] < distance[i]) {
				t = distance[i];
				distance[i] = distance[i + 1];
				distance[i + 1] = t;
			}
		}
	}

	printf("%d\n",numlines);

	for (i=0; i<numlines; i++) {
		printf("%s", line[i]);
	}

	for (i=0; i<numlines; i++) {
		printf("%d\n",distance[i]);
	}

	return 0;
}
