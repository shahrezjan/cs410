#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <sys/stat.h>
#include <string.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <err.h>

// HTTP-Response length maxiumu size
static const int MAX_RESLEN = 1024;
static const int MAX_REQLEN = 1024;

// Maximum number of simultaneous connections allowed
static const int MAX_SCONN = 10;

// ------------------------------------------------------
// HTTP RESPONSE HEADERS
// ------------------------------------------------------

char general_resheader[] = "HTTP/1.1 200 OK\n";

char txt_response[] = "HTTP/1.1 200 OK\n"
"Content-Type: text/plain; charset=UTF-8\n\n"
"%s\n";

char txt_resheader[] = "HTTP/1.1 200 OK\r\n"
"Content-Type: text/plain; charset=UTF-8\r\n\r\n";

char htmlfile_response[] = "HTTP/1.1 200 OK\n"
"Content-Type: text/html; charset=UTF-8\n\n"
"%s\n";

char htmlfile_resheader[] = "HTTP/1.1 200 OK\n"
"Content-Type: text/html; charset=UTF-8\n\n";

char jpgjpeg_response[] = "HTTP/1.1 200 OK\r\n"
"Content-Type: image/jpeg\r\n"
"Content-Length: %li\r\n\r\n";

char gif_response[] = "HTTP/1.1 200 OK\r\n"
"Content-Type: image/gif\r\n"
"Content-Length: %li\r\n\r\n";

char nonexistent_reponse[] = "HTTP/1.1 404 Not Found\n"
"Content-Type: text/plain; charset=UTF-8\n\n"
"404 Error: %s not found!\n";

char notimplemented_reponse[] = "HTTP/1.1 501 Not Supported\n"
"Content-Type: text/plain; charset=UTF-8\n\n"
"501 Error: %s not supported!\n";

// END HTTP RESPONSE HEADERS ----------------------------

// Generate HTML-reponse
int generate_response(char *boilerplate, char *buffer, char *body) {
    sprintf(buffer, boilerplate, body);
    return 0;
}

/* Parses the 'REQUEST' portion from an HTTP get in the form:
    http://ip.address.of.server:port-number/REQUEST
   
   Value is stored in buffer
*/
int parse_get_request(char *buffer, char *getreqraw) {
    /* Parse get request */
    char bufcpy[strlen(getreqraw)+1];
    strcpy(bufcpy, getreqraw);
    char *firstline;
    char *token;
    char *request;
    // get first line
    firstline = strtok(bufcpy, "\n");
    // 'GET'
    token = strtok(firstline, " ");
    // [REQUEST] ex: '/somedirectory'
    token = strtok(NULL, " ");

    char bufcpy2[strlen(token)+4];
    strcpy(bufcpy2, ".");
    strcat(bufcpy2, token);

    strcpy(buffer, bufcpy2);
}


int main (int argc, char *argv[])
{
	/* Parse port number */
	uint16_t hostordpn = (uint16_t) atoi(argv[1]);

	/* Convert port num to network byte order */
	uint16_t hostpn = htons(hostordpn);
	
	/* Create a socket */
	int sock;
	socklen_t addrlen;

	/* Initialize socket endpoints */
	int bufsize = 2048;
	char *buffer = malloc(2048);
	struct sockaddr_in hostaddress, clientaddress;
    socklen_t sin_len = sizeof(clientaddress);
    int client_fd;
    int one = 1;

    /* Prepare socket */
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (socket < 0) {
       perror("socket error: ");
       exit(1);
 	}
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(int));
    hostaddress.sin_family = AF_INET;
    hostaddress.sin_addr.s_addr = INADDR_ANY;
    hostaddress.sin_port = hostpn;
   
   	/* Bind socket */
    if (bind(sock, (struct sockaddr *) &hostaddress, sizeof(hostaddress)) == -1) {
        close(sock);
        perror("bind error:");
        exit(1);
    }

 	/* Begin listening for connections and requests */
    if (listen(sock, MAX_SCONN) < 0) {
    	perror("listen error:");
    	exit(1);
    }


    while (1) {

    	/* Wait for connections */
        client_fd = accept(sock, (struct sockaddr *) &clientaddress, &sin_len);
 
      	if (client_fd == -1) {
        	perror("Connection error:");
        	continue; 
      	}

        /* Now we have a valid connection with client */

        pid_t pid = fork();

        if(pid == 0) {

            /* Child Process (i.e. one client connection) */

            close(sock);
            memset(buffer, 0, 2048);            // Ensures null-terminated write
            read(client_fd, buffer, 2047);      // Read GET-request into buffer
            printf("%s\n", buffer);

            char request[MAX_REQLEN];
            parse_get_request(request, buffer);

            // Parse out portion used as arguments for cgi script
            // Will be left as NULL if there are 0 arguments
            int flag_args = 0;
            char cgi_args_raw[MAX_REQLEN];
            char *token;
            token = strtok(request, "?");
            strcpy(request,token);
            token = strtok(NULL, "");
            if (token != NULL) {
                strcpy(cgi_args_raw,token);
                flag_args = 1;
            }

            /* Will hold the filled out HTTP response */
            char resfilled[MAX_RESLEN];

            /* Determine what type of file the client requested */
            struct stat path_stat;
            if (stat(request, &path_stat) < 0 ) {
                /* Non-existent file -- send HTTP 404 error */
                generate_response(nonexistent_reponse, resfilled, request);
                write(client_fd, resfilled, strlen(resfilled));

            }
            if (S_ISDIR(path_stat.st_mode) == 1) {

                /* Directory file -- list all entries */

                char readbuf[80];
                char command[80];
                sprintf(command, "ls -l %s", request);

                FILE *pipeoutput;

                if( (pipeoutput = popen(command, "r")) == NULL ) {
                    perror("popen error: ");
                    exit(1);
                }

                write(client_fd, txt_resheader, strlen(txt_resheader));

                while(fgets(readbuf, 80, pipeoutput)) {
                    write(client_fd, readbuf, strlen(readbuf));
                }

                // Close pipe
                if (pclose(pipeoutput) < 0) {
                    perror("pclose error: ");
                    exit(1);
                }
            }
            else if(S_ISREG(path_stat.st_mode) == 1) {
                
                /* Regular file */

                FILE *regfile = fopen(request, "rb");

                // Determine file type
                char *extension;
                extension = strrchr(request, '.');

                if ( strcmp(extension, ".jpeg") == 0 || strcmp(extension, ".jpg")  == 0 ||\
                            strcmp(extension, ".gif") == 0 ) {

                    /* JPEG or JPG or GIF */

                    // Buffer to transfer data from file to response
                    char binarybuf[80];

                    // Read entire binary file into a buffer
                    fseek(regfile, 0L, SEEK_END);
                    long picsize = ftell(regfile);
                    rewind(regfile);
                    // binarybuf = malloc(picsize);

                    if (strcmp(extension, ".jpeg")== 0 || strcmp(extension, ".jpg")==0) {
                        sprintf(resfilled, jpgjpeg_response, picsize);
                        write(client_fd, resfilled, strlen(resfilled));
                    } else {
                        sprintf(resfilled, gif_response, picsize);
                        write(client_fd, resfilled, strlen(resfilled)); 
                    }

                    // Read 80 bytes at a time
                    long leftover = picsize;
                    fread(binarybuf, 80, 1, regfile);
                    write(client_fd, binarybuf, 80);
                    leftover = leftover - 80;
                    while(leftover > 80) {
                        fread(binarybuf, 80, 1, regfile);
                        write(client_fd, binarybuf, 80);
                        leftover = leftover - 80;
                    }
                    // Read left over bytes
                    fread(binarybuf, leftover, 1, regfile);
                    write(client_fd, binarybuf, leftover);
                    leftover = leftover - leftover;

                    if (leftover != 0) {
                        perror("Error reading/writing binary data");
                        exit(1);
                    }

                }
                else if (strcmp(extension, ".html") == 0) {

                    /* HTML */

                    int max_linesize = 128;
                    char linebuf[max_linesize];

                    strcpy(resfilled, "");
                    strcat(resfilled, htmlfile_resheader);
                    while( fgets(linebuf, max_linesize, regfile) != NULL ) {
                        strcat(resfilled, linebuf);
                    }
                    write(client_fd, resfilled, strlen(resfilled));
                }
                else if (strcmp(extension, ".cgi") == 0) {

                    /* CGI */

                    // Run the cgi script using popen and wrap output in an html file
                    // Change access permissions to script first
                    char readbuf[80];
                    char command[400];
                    sprintf(command, "chmod 755 %s && %s", request, request);
                    // printf("START%sSTART\n", command);

                    // Parse arguments for cgi script if any (indicated by flag)
                    if (flag_args == 1) {
                        char tempbuf[80];
                        char *token1;
                        char *token2;
                        token1 = strtok(cgi_args_raw, "&");
                        while (token1 != NULL) {
                            strcpy(tempbuf, token1);
                            token2 = strrchr(tempbuf, '=');
                            strcat(command, " ");
                            strcat(command, &token2[1]);
                            token1 = strtok(NULL, "&");
                        }
                    }

                    FILE *pipeoutput;

                    if( (pipeoutput = popen(command, "r")) == NULL ) {
                        perror("popen error: ");
                        exit(1);
                    }

                    write(client_fd, general_resheader, strlen(general_resheader));

                    while(fgets(readbuf, 80, pipeoutput)) {
                        write(client_fd, readbuf, strlen(readbuf));
                    }

                    // Close pipe
                    if (pclose(pipeoutput) < 0) {
                        perror("pclose error: ");
                        exit(1);
                    }

                }
                else {
                    // File exists but is not supported - send 501 error
                    generate_response(notimplemented_reponse, resfilled, request);
                    write(client_fd, resfilled, strlen(resfilled));
                }

                fclose(regfile);
            }

            close(client_fd);
            exit(0);
        }

        /* Parent Process */
        close(client_fd);

    } // End while loop

	return 0;
}
