
all: getsyms_dl getsections_dl

getsyms_dl: getsyms_dl.c libobjdata.so
	gcc -I./include -L./lib -o getsyms_dl getsyms_dl.c ./lib/libobjdata.so -lbfd -ldl

getsections_dl: getsections_dl.c libobjdata.so
	gcc -I./include -L./lib -o getsections_dl getsections_dl.c ./lib/libobjdata.so -lbfd -ldl

libobjdata.so: objsym.o objsect.o
	gcc -shared -o libobjdata.so objsym.o objsect.o -lbfd
	mv libobjdata.so ./lib

objsym.o: objsym.c
	gcc -c -fPIC -o objsym.o objsym.c -lbfd

objsect.o: objsect.c
	gcc -c -fPIC -o objsect.o objsect.c -lbfd

clean:
	rm -f objsym.o objsect.o ./lib/libobjdata.so getsyms_dl getsections_dl










