
all: arduinoGetTemp webserv

arduinoGetTemp: arduinoGetTemp.c
	gcc -o arduinoGetTemp arduinoGetTemp.c

webserv: webserv.c
	gcc -o webserv webserv.c

clean:
	rm -rf *~ arduinoGetTemp webserv
