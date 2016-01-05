/*
 * Hwa-seung Erstling
 * CS 410 - Optional Assignment 3
 *
 * Code for serial port config/connection taken and modified from:
 * http://chrisheydrick.com/2012/06/17/how-to-read-serial-data-from-an-arduino-in-linux-with-c-part-3/
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>

/* Arduino R3 is on /dev/ttyACM0 */
char *portname = "/dev/ttyACM0";
 
int main(int argc, char *argv[])
{
	/* Open the arduino device file in non-blocking mode */
	int fd;
	fd = open(portname, O_RDWR | O_NOCTTY);

	// Serial port init/config ----------------------------------

	/* Set up the control structure */
	struct termios toptions;

	/* Get currently set options for the tty */
	tcgetattr(fd, &toptions);

	/* Set custom options */

	/* 9600 baud */
	cfsetispeed(&toptions, B9600);
	cfsetospeed(&toptions, B9600);
	/* 8 bits, no parity, no stop bits */
	toptions.c_cflag &= ~PARENB;
	toptions.c_cflag &= ~CSTOPB;
	toptions.c_cflag &= ~CSIZE;
	toptions.c_cflag |= CS8;
	/* no hardware flow control */
	toptions.c_cflag &= ~CRTSCTS;
	/* enable receiver, ignore status lines */
	toptions.c_cflag |= CREAD | CLOCAL;
	/* disable input/output flow control, disable restart chars */
	toptions.c_iflag &= ~(IXON | IXOFF | IXANY);
	/* disable canonical input, disable echo,
	disable visually erase chars,
	disable terminal-generated signals */
	toptions.c_iflag &= ~(ICANON | ECHO | ECHOE | ISIG);
	/* disable output processing */
	//toptions.c_oflag &= ~OPOST;

	/* wait for 24 characters to come in before read returns */
	toptions.c_cc[VMIN] = 12;
	/* no minimum time to wait before read returns */
	toptions.c_cc[VTIME] = 1;

	/* commit the options */
	tcsetattr(fd, TCSANOW, &toptions);

	/* Flush anything already in the serial buffer */
	tcflush(fd, TCIFLUSH);

	// End serial port init/config -------------------------------


	/* 
	 * GET TEMPERATURE READING FROM ARDUINO 
	 */

	int n;
	char charbuf[2] = "";
	// first flush the serial buffer
	tcflush(fd, TCIFLUSH);

	// Send a signal to arduino to print the next weather reading to serial output
	write(fd, "c", 1);

	while (1) {
	 	if ((n = read(fd,charbuf,1)) > 0) {

	 		// Arduino will send a newline (only once) when recieving first
	 		// byte from serial device (server).
	 		// Resend the command to get-temperature.
			// There were issues with the first read, so execute another read
			// if this is the first time communicating with arduino since startup.
			if (strcmp(charbuf, "\n") == 0) {
				char *args[] = {"./arduinoGetTemp", 0};
				if (execvp("./arduinoGetTemp", args) < 0) {
					perror("execvp error: ");
					exit(1);
				}
			}

			// Reached end of weather output, stop reading from serial port	
			// Arduino will send a delim char to denote end of output
			if (strcmp(charbuf, ";") == 0) {
				break;
			}
			printf("%s", charbuf);
		}
	}
	printf("\n");

	close(fd);
return 0;
}
