#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <signal.h>
#include <setjmp.h>

static const int MAX_COMMANDS = 20;
static int pipe1[2];
static int pipe2[2];
static int pipepos = 0;

sigjmp_buf prompt_buf;
volatile static int cpids[50];

// Holds flag options from command line input
struct FlagOpts {
	int fmt1; int fmt2;
	int fmt3; int fmt4;
	int fmt5; int fmt6;
	int fmt7; int fmt8;
	
	int pord1, pord2, pord2c, pord3;
};

void flags_init(struct FlagOpts *flags) {
	flags->fmt1 = 0; flags->fmt2 = 0; 
	flags->fmt3 = 0; flags->fmt4 = 0;
	flags->fmt5 = 0; flags->fmt6 = 0; 
	flags->fmt7 = 0; flags->fmt8 = 0;
	
	flags->pord1 = 0;
	flags->pord2 = 0;
	flags->pord2c = 0;
	flags->pord3 = 0;
}

void sigchld_handler(int signum) {
	int status;
	// keep looping until there are no SIGCHLD statuses
	// for any running child processes
	while( waitpid(-1, &status, WNOHANG) > 0 ) { }
}

void sigint_handler (int signum) {
	int i;
	for (i = 0; i < 50; i++) {
		if (cpids[i] == -1) { continue; }
		if ( kill(cpids[i], SIGINT) < 0 ) {
			perror("kill error: ");
			exit(1);
		}
		cpids[i] = -1;
	}
	if(isatty(0)) fprintf(stdout, "\n");
	siglongjmp(prompt_buf, 1);	
}

int freeslot() {
	int i;
	for(i=0;i<50;i++) {
		if(cpids[i]==-1) { return i; }
	}
	return -1;
}

void run_command(char *command, struct FlagOpts *flags);
void run_process(char **argtokens, char *filename, struct FlagOpts *flags);
void tokenize_cmd(char *cmdline, char **argtokens);
int check_gtprefix(char **argtokens, char *str);
int check_background(char **argtokens); 
void setup_pipes(char *command, struct FlagOpts *flags);

int main (int argc, char **argv)
{
	struct sigaction sigint_action, sigchld_action; 

	// Signal action for SIGINT
	sigint_action.sa_handler = sigint_handler;
	sigint_action.sa_flags = SA_RESTART | SA_NOCLDSTOP;
	if( (sigaction(SIGINT, &sigint_action, NULL)) < 0 ) {
		perror("sigaction error: ");
		exit(1);
	}

	// Signal action for SIGCHLD
	sigchld_action.sa_handler = sigchld_handler;
	sigchld_action.sa_flags = SA_RESTART | SA_NOCLDSTOP;
	if( (sigaction(SIGCHLD, &sigchld_action, NULL)) < 0 ) {
		perror("sigaction error: ");
		exit(1);
	}

	struct FlagOpts *flags = malloc(sizeof(struct FlagOpts));
	char *cmds[MAX_COMMANDS+1];
	int c;

	while (sigsetjmp(prompt_buf, 1) != 0) { }

	while(1) {
		int i;
		for (i = 0; i < 50; i++) {
			cpids[i] = -1;
		}

		char *cmdline = NULL;
		ssize_t linebufsize = 0;

		if (isatty(0)) {
			fprintf(stdout, "myshell> ");
		}

		// Either Ctrl-D inputted (returning an EOF to stdin),
		// OR... EOF reached in file
		if (getline(&cmdline, &linebufsize, stdin) == -1 ) { 
			if (isatty(0)) fprintf(stdout, "\n");
			break; 
		}

		// Enter-key pressed - prompt new line to be read
		if (cmdline[0] == '\n') { continue; }		

		// Parse each individual command separated by ";"
		int pos = 0;
		char *token;
		token = strtok(cmdline, ";");
		while(token != NULL) {
			if(pos >= MAX_COMMANDS) {
				perror("Error: Too many commands\nRunning first 20 commands...\n");		
				break;
			}
			cmds[pos] = token;
			pos++;
			token = strtok(NULL, ";");
		}
		cmds[pos] = NULL;	// terminate array of commands
		
		// Run each command separately
		pos = 0;
		while (cmds[pos] != NULL) {
			flags_init(flags);
			if (strstr(cmds[pos], "|") != NULL) { 
				setup_pipes(cmds[pos], flags);
			}
			else { run_command(cmds[pos], flags); }
			pos++;
		}
	
	} // end myshell> while loop

	return 0;
}

void setup_pipes(char *command, struct FlagOpts *flags) {
	int pos = 0;
	int size = 0;
	char *token;
	char *pipeline[MAX_COMMANDS+1];
	token = strtok(command, "|");
	while (token != NULL) {
		pipeline[pos] = token;
		pos++;
		token = strtok(NULL, "|");
	}
	pipeline[pos] = NULL;
	size = pos;

    // Set up pipes	
	pipe(pipe1);
	pipe(pipe2);

	/*
		Case: "cmd1 | cmd2"
		Only needs one pipe
	*/
	if (size == 2) {
        flags_init(flags);
		flags->pord1 = 1;
		run_command(pipeline[0], flags);

        flags_init(flags);
		flags->pord2 = 1;	
		run_command(pipeline[1], flags);	
	}
	/*
		Case: "cmd1 | cmd2 | cmd3"
		Only two pipes
	*/
	else if (size = 3) {
        flags_init(flags);
		flags->pord1 = 1;
		run_command(pipeline[0], flags);
		
        flags_init(flags);
		flags->pord2c = 1;	
		run_command(pipeline[1], flags);
		
		pipepos = pos;
        flags_init(flags);
		flags->pord3 = 1;
		run_command(pipeline[2], flags);
	}
	else { 
		perror("error: need 2 or 3 commands to pipe"); 
	}

}

void run_command(char *command, struct FlagOpts *flags) {
	int argbufsize = 64;	// Maximum number of arguments
	int pos = 0;
	char **argtokens = malloc(argbufsize * sizeof(char*));
	char *token;
	char *prog;
	char *filename;
    char *temp1, *temp2;

	if (strstr(command, ">") != NULL) {
		prog = strtok(command, ">");
		filename = strtok(NULL, ">");
        temp1 = strtok(filename, " \n");
        temp2 = strtok(NULL, " \n");
        if (temp2 != NULL && (strcmp(temp2, "&") == 0) ) {
            flags->fmt8 = 1;
            filename = temp1;
        }
		if (prog == NULL || filename == NULL) {
			perror("error: invalid program or filename");
			exit(1);
		}
		tokenize_cmd(prog, argtokens);
		if(check_gtprefix(argtokens, "1"))
			flags->fmt3 = 1;
		else if (check_gtprefix(argtokens, "2"))
			flags->fmt6 = 1;
		else if (check_gtprefix(argtokens, "&"))
			flags->fmt7 = 1;
		else
			flags->fmt3 = 1;

		run_process(argtokens, filename, flags);
	}
	else if (strstr(command, "<") != NULL) {
		prog = strtok(command, "<");
		filename = strtok(NULL, "<");
		if (prog == NULL || filename == NULL) {
			perror("error: invalid program or filename");
			exit(1);
		}
        temp1 = strtok(filename, " \n");
        temp2 = strtok(NULL, " \n");
        if (temp2 != NULL && (strcmp(temp2, "&") == 0) ) {
            flags->fmt8 = 1;
            filename = temp1;
        }
		tokenize_cmd(prog, argtokens);
		flags->fmt4 = 1;
		run_process(argtokens, filename, flags);
	}
	else {	// Run program with arguments
		tokenize_cmd(command, argtokens);
        if( check_background(argtokens) == 1 ) {
            flags->fmt8 = 1;
        }
		run_process(argtokens, NULL, flags);
	}
}

void run_process(char **argtokens, char *filename, struct FlagOpts *flags) {
	pid_t pid, wpid;
	int status;	
	int filenum;
	char *fnp;

	pid = fork();
	if (pid == 0) {		// child process
        fnp = strtok(filename, " \n");
		if(flags->pord1) {
			dup2(pipe1[1], 1);
			close(pipe1[0]);
		}
		else if(flags->pord2) {
			dup2(pipe1[0], 0);
			close(pipe1[1]);
		} 
		else if(flags->pord2c) {
			dup2(pipe1[0], 0);
			dup2(pipe2[1], 1);
			close(pipe1[1]);
			close(pipe2[0]);
		}
		else if(flags->pord3) {
			dup2(pipe2[0], 0);
			close(pipe1[0]);
			close(pipe1[1]);
			close(pipe2[1]);
		}
		else ;

		if(flags->fmt4) {
			filenum = open(fnp, O_RDONLY);
			dup2(filenum, 0);
		}
		if(flags->fmt3 || flags->fmt6 || flags->fmt7) {
			filenum = open(fnp, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
			if (flags->fmt3)
				dup2(filenum, 1);
			if (flags->fmt6)
				dup2(filenum, 2);
			if (flags->fmt7) {
				dup2(filenum,1);
				dup2(filenum,2);
			}
		}

		if(execvp(argtokens[0],argtokens) == -1) {
			perror("execvp error: ");
			exit(0);
		}
	} else if (pid < 0) {
		perror("fork error: ");
		exit(0);
	} else {
		// Parent process
		int k;
		if ( (k = freeslot()) < 0) {
			perror("error");
			exit(1);
		}
		cpids[k] = pid;
        if(flags->pord1) {
            close(pipe1[1]);
        }
        if(flags->pord2) {
            close(pipe1[0]);
        }
        if(flags->pord2c) {
            close(pipe2[1]);
        }
        if(!flags->fmt8) {
		    do {
			    wpid = waitpid(pid, &status, WUNTRACED);
		    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
        }
	}
}

void tokenize_cmd(char *cmdline, char **argtokens) {
	char *token;		
	int pos = 0;
	token = strtok(cmdline, " \n");
	while (token != NULL) {
		argtokens[pos] = token;
		pos++;
		token = strtok(NULL, " \n");
	}
	// place null ptr at end of array for execv()
	argtokens[pos] = NULL;
}

// Return 1 if last non-null string is same as prefix
// ...and replace prefix string with NULL
// Otherwise, return 0
int check_gtprefix(char **argtokens, char *prefix) {
	int pos = 0;
	while (argtokens[pos] != NULL) {
		if ( strcmp(argtokens[pos], prefix) == 0 ) {
			if (argtokens[pos+1] == NULL) {
				argtokens[pos] = NULL;
				return 1;
			}
		}
		pos++;
	}
	return 0;
}

// Return 1 if last non-null string in argtokens is "&"
int check_background(char **argtokens) {
	int pos = 0;
	while (argtokens[pos] != NULL) {
		if ( strcmp(argtokens[pos], "&") == 0 ) {
			if (argtokens[pos+1] == NULL) {
				argtokens[pos] = NULL;
				return 1;
			}
		}
		pos++;
	}
	return 0;
}



