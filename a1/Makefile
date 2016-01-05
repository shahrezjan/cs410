
all: finds

finds: finds.c my_printf.o
	gcc -m32 -o finds finds.c my_printf.o

my_printf.o: my_printf.c
	gcc -m32 -c -o my_printf.o my_printf.c

clean:
	rm -f finds my_printf.o *~










