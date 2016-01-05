#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main (int argc, char *argv[])
{

	char *filename = argv[1];

	int i;

	char command[400];
	char readbuf[80];
	char writebuf[80];

	// For each pattern, run 'grep -e' on the file
	for (i = 2; i < argc; i++) {
	
		strcpy(command, "grep -o -e ");
		strcat(command, argv[i]);
		strcat(command, " ");
		strcat(command, filename);
		strcat(command, " | wc -w");

		// printf("COMMAND%sCOMMAND\n", command);


	    FILE *pipeoutput;

	    if( (pipeoutput = popen(command, "r")) == NULL ) {
	        perror("popen error: ");
	        exit(1);
	    }

	    // write(client_fd, txt_response, strlen(txt_response));

	    while(fgets(readbuf, 80, pipeoutput)) {
	    	sprintf(writebuf, "%s %s", argv[i], readbuf);
	        write(1, writebuf, strlen(writebuf));
	    }

	    // Close pipe
	    if (pclose(pipeoutput) < 0) {
	        perror("pclose error: ");
	        exit(1);
	    }
	}

	return 0;
}
