#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <limits.h>
#include <fcntl.h>
#include "my_printf.h"

/* References:
 * http://www.gnu.org/software/libc/manual/html_node/Example-of-Getopt.html#Example-of-Getopt
 * Program 4.7 in Advanced Programming in the UNIX Environment
 * West's my_printf example from class
 */ 

// Holds flag options from command line input
struct FlagOpts {
	int c;
	int h;
	int S;	
	int l;
};

// For linked list
struct Node {
	ino_t x;
	struct Node *next;
};

// Function definitions
static int myftw(char *, char *, struct FlagOpts *);
static int dopath(char *, struct FlagOpts *);
static int readfile(const char *pathname, char *searchstr, const struct stat *statptr, int type);
static int checkEnding(char c, char *filename);
static int mygrep (char *ss, int size_ss, char *line, int size_line);
static int isValidGrepStr(char *str);
static int isAlphaNumeric (char *cc);
static int isASCII (char *cc); 
static int markVisited(ino_t inodenum);
static int alreadyVisited(ino_t inodenum);
static int fileTypesFlag = 0; 

//----------------------------------------------------------------------

int main (int argc, char **argv)
{
	// parse program parameters
	char *pathname  = NULL;
	char *searchstr = NULL;
	char *filetypes = NULL;
	int index;
	int c;

	struct FlagOpts flags;
	flags.c = 0; flags.h = 0; flags.S = 0; flags.l = 0;

	opterr = 0;
	while ((c = getopt (argc, argv, "p:f:l::s:")) != -1)
	switch (c)
	  {
	  case 'p':
				pathname = optarg;
		break;
			case 'f':
				fileTypesFlag = 1;
				filetypes = optarg;
				if ( mygrep( "c" , 1, filetypes, strlen(filetypes)) )
					flags.c = 1;
				if ( mygrep( "h" , 1, filetypes, strlen(filetypes)) )
					flags.h = 1;
				if ( mygrep( "S" , 1, filetypes, strlen(filetypes)) )
					flags.S = 1;
				break;
			case 'l':
				flags.l = 1;
				break;
	  case 's':
				searchstr = optarg;
				if ( isValidGrepStr(searchstr) == 0 ) {
					my_errprintf("Invalid argument for [-s]: %s\n", searchstr );
					exit(EXIT_FAILURE);
				}
		break;
	  case '?':
		if (optopt == 'f')
		  my_errprintf ("Option -%c requires an argument.\n", optopt);
		else if (isprint (optopt))
		  my_errprintf ("Unknown option `-%c'.\n", optopt);
		else
		  my_errprintf ("Unknown option character `\\x%x'.\n", optopt);
		return 1;
	  default:
		abort ();
	  }

	for (index = optind; index < argc; index++)
	my_printf ("Non-option argument %s\n", argv[index]);

	// Run program with parsed input
	myftw(pathname, searchstr, &flags);

	return 0;
}

//----------------------------------------------------------------------
/*
* Descend through the hierarchy, starting at "pathname".
* The caller's func() is called for every file.
*/
#define FTW_F 1 /* file other than directory */
#define FTW_D 2 /* directory */
#define FTW_DNR 3 /* directory that can't be read */
#define FTW_NS 4 /* file that we can't stat */

static char *fullpath;   /* contains full pathname for every file */
static char *symlpath;	 /* contains name of real path for symb. link */

struct Node *root; /* linked list of visited i-nodes */
struct Node *iter; /* ptr to iterate through list */

static int myftw(char *pathname, char *searchstr, struct FlagOpts *flags)
{
	int len;
	fullpath = malloc(PATH_MAX+1) ; /* malloc's for PATH_MAX+1 bytes */
	symlpath = malloc(PATH_MAX+1) ;

	root = (struct Node *) malloc( sizeof(struct Node) );
	root->x = -1;
	root->next = NULL;

	strncpy(fullpath, realpath(pathname,NULL), PATH_MAX+1);
	return(dopath(searchstr, flags));
}

//----------------------------------------------------------------------
/*
* Descend through the hierarchy, starting at "fullpath".
* If "fullpath" is anything other than a directory, we lstat() it,
* call the search function, and return. For a directory, we call ourself
* recursively for each name in the directory.
*/
static int /* we return whatever func() returns */

dopath(char *searchstr, struct FlagOpts *flags)
{
	struct stat statbuf;
	struct dirent *dirp;
	DIR *dp;
	int ret;
	char *ptr;
	
	int errnum;
	errno = 0;

	if (lstat(fullpath, &statbuf) < 0) { /* stat error */
		errnum = errno;
		my_errprintf("Error reading file attribute: %s\n", strerror(errnum) );
	}

	if (S_ISLNK(statbuf.st_mode) != 0) { /* symbolic link */
		if( (*flags).l == 1) { // search symbolic links
			int k = readlink( fullpath, symlpath, PATH_MAX+1 );
			symlpath[k] = '\0';
			if (stat(fullpath, &statbuf) < 0) { /* stat error */
				errnum = errno;
				my_errprintf("Error reading file attribute: %s\n", strerror(errnum) );
			}
		} 
		else { // do not search file or folder
			return 0;
		}
	}

	// Do not read already visited files
	if ( alreadyVisited(statbuf.st_ino) == 1 ) {
		return 0;
	}
	
	if (S_ISDIR(statbuf.st_mode) == 0) { /* not a directory */
		// Skip certain files according to [-f] flags used
		if ( fileTypesFlag == 1 )
		{
			if ( checkEnding('c',fullpath)==1 ) {
				if( flags->c == 0) return 0;
			}
			else if ( checkEnding('h',fullpath)==1 ) {
				if( flags->h == 0) return 0;
			}
			else if ( checkEnding('S',fullpath)==1 ) {
				if( flags->S == 0) return 0;
			}
			else { //other type of file - skip since flag opt used
				return 0;
			}
		}

		// add regular file i-node to list of files visited
		markVisited(statbuf.st_ino);
		return(readfile(fullpath, searchstr, &statbuf, FTW_F));
	}

	/*
* It's a directory. First call func() for the directory,
* then process each filename in the directory.
*/
	// add directory i-node to list of files visited
	markVisited(statbuf.st_ino);

	if ((ret = readfile(fullpath, searchstr, &statbuf, FTW_D)) != 0)
		return(ret);

	ptr = fullpath + strlen(fullpath); /* point to end of fullpath */
	*ptr++ = '/';
	*ptr = 0;

	if ((dp = opendir(fullpath)) == NULL) /* can't read directory */
		return(readfile(fullpath, searchstr, &statbuf, FTW_DNR));

	while ((dirp = readdir(dp)) != NULL) {
		if (strcmp(dirp->d_name, ".") == 0 ||
				strcmp(dirp->d_name, "..") == 0)
			continue; /* ignore dot and dot-dot */
		strcpy(ptr, dirp->d_name); /* append name after slash */
		if ((ret = dopath(searchstr, flags)) != 0) /* recursive */
			break; /* time to leave */
	}

	ptr[-1] = 0; /* erase everything from slash onwards */
	if (closedir(dp) < 0) ;
	return(ret);
}


//----------------------------------------------------------------------

// function to read regular file
static int readfile(const char *pathname, char *searchstr, const struct stat *statptr, int type)
{
	// file descriptor
	int fd;
	char charbuf[1];	
	char linebuf[LINE_MAX];
	int lbpos = 0;
	int errnum;
	
	// error handling: if fd==-1, error opening file
	if ( (fd = open(pathname, O_RDONLY)) < 0 ) {
		errnum = errno;
		my_errprintf("Error opening file: %s\n", strerror(errnum) );
	}

	// Copy one line to buffer by reading file one byte
	//  at a time until a newline character is reached
	//	OR buffer is full

	while ( read(fd, charbuf, 1) > 0 ) {
		if ( isASCII(charbuf) == 0 ) // contains non-ascii char, skip file 
			break;

		// New line character...
		// 	-put null char at end of line
		//		then pass to mygrep and print line if match
		if ( (charbuf[0] == '\n') || (lbpos >= LINE_MAX-1)) {
			linebuf[lbpos] = '\0';
			if ( mygrep(searchstr, strlen(searchstr), linebuf, strlen(linebuf)) == 1 ) { 
				my_printf("Line: %s\nFile: %s\n", linebuf, pathname);
			}

			lbpos = 0;

		} else { // store byte in linebuf
			linebuf[lbpos] = (char) charbuf[0];
			lbpos++ ;
		}
	}

	close(fd);
	return 0;
}

//----------------------------------------------------------------------

/* Regular expression evaluator
 * 	--> size parameters are number of bytes in string excluding null char
 */
static int mygrep (char *ss, int size_ss, char *line, int size_line) 
{
	int sspos    = 0;
	int linepos  = 0;
	int i        = 0;

	while (sspos <= size_ss && i <= size_line && linepos < size_line) {
		if (sspos == size_ss) return 1;

		if ( isAlphaNumeric(line+i) == 0 ) {   // Non-alphanumeric character
			linepos++;
			i = linepos;			
			sspos = 0;
			continue;
		}

		if ( line[i] == '\0' ) {
			linepos++;
			i = linepos;
			sspos = 0;
			continue;
		}
		else if ( ss[sspos] == '.' ) {
			if ( isAlphaNumeric(line+i) ) {
				i++;
				sspos++;
			}
		}
		else if ( isAlphaNumeric(ss+sspos) == 1 ) {
			if ( (sspos+1 <= size_ss) && ((isAlphaNumeric(ss+sspos+1) == 1) || 
					(ss[sspos+1] == '\0') || (ss[sspos+1] == '.')) ) {
				if ( ss[sspos] == line[i] ) { // MATCH
					sspos++ ;
					i++ ;
					continue;
				} else {
					linepos++;
					i = linepos;
					sspos = 0;
				}
			} else if ( (sspos+1 < size_ss) && (ss[sspos+1] == '*') ) {
				while ( ss[sspos] == line[i] ) 
					i++ ;
				sspos += 2;
				continue;
			} else if ( (sspos+1 < size_ss) && (ss[sspos+1] == '?') ) {
				if ( ss[sspos] == line[i] ) {
					i++;
					sspos += 2;
					continue;
				} else {
					sspos += 2;
					continue;
				}
			} else ;
		} else ;
	//end while loop
	}

	return 0;
}

static int isValidGrepStr(char *str) {
	int i = 0;
	while (str[i] != '\0') {
		if ( isAlphaNumeric(str+i) || str[i]=='.' ||
					str[i]=='*' || str[i]=='?' ) i++;
		else return 0;
	}
	return 1;
}

static int isAlphaNumeric (char *cc) 
{
	if ( (cc[0] >= 48 && cc[0] <= 57) || 			// alphanumeric ASCII values 
				(cc[0] >= 65 && cc[0] <= 90) || 		// 
				(cc[0] >= 97 && cc[0] <= 122) ) {		// 
		return 1;
	} else {
		return 0;
	}	
}

static int isASCII (char *cc) 
{
	if ( cc[0] >= 0 && cc[0] <= 127 ) 
		return 1;
	
	return 0;
}

static int markVisited(ino_t inodenum) {
	// create node with inode number
	struct Node *temp;
	temp = (struct Node *) malloc( sizeof(struct Node) );
	temp->x = inodenum;
	temp->next = NULL;

	// move iterator to end of list	
	iter = root;
	while (iter->next != NULL) {
		iter = iter->next;
	}
	// link new node at end of list
	iter->next = temp;
	iter = root;
}

static int alreadyVisited(ino_t inodenum) {
	iter = root;
	while (iter->next != NULL) {
		iter = iter->next;
		if ( iter->x == inodenum )
			return 1;
	}
	return 0;
}

static int checkEnding(char c, char *filename) {
	int i = 0;
	while (filename[i] != '.') {
		if (filename[i] == '\0' || isASCII(filename+i)==0) 
			return 0;	
		i++;
	}
	i++;
	if ( filename[i] == c && filename[i+1] == '\0' )
		return 1;

	return 0;	
}


