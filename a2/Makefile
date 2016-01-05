
all: myshell find_paths tsp_p tsp_t tspsort

myshell: myshell.c
	gcc -o myshell myshell.c

find_paths: find_paths.c
	gcc -o find_paths find_paths.c

tsp_p: tsp_p.c
	gcc -o tsp_p tsp_p.c

tsp_t: tsp_t.c
	gcc -o tsp_t tsp_t.c

tspsort: tspsort.c
	gcc -o tspsort tspsort.c

clean:
	rm -f myshell find_paths tsp_p tsp_t tspsort *~
