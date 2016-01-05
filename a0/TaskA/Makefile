
all: getsyms getsections

getsyms: getsyms.c libobjdata.a
	gcc -I./include -L./lib -o getsyms getsyms.c ./lib/libobjdata.a -lbfd

getsections: getsections.c libobjdata.a
	gcc -I./include -L./lib -o getsections  getsections.c ./lib/libobjdata.a -lbfd

libobjdata.a: objsym.o objsect.o
	ar rs libobjdata.a objsect.o objsym.o
	mv libobjdata.a ./lib
	ranlib ./lib/libobjdata.a

objsym.o: objsym.c
	gcc -c -o objsym.o objsym.c -lbfd

objsect.o: objsect.c
	gcc -c -o objsect.o objsect.c -lbfd

clean:
	rm -f objsym.o objsect.o ./lib/libobjdata.a getsyms getsections










