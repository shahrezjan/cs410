#include <stdio.h>
#include <bfd.h>
#include <objsect.h>
#include <dlfcn.h>

#define rdtsc(x)      __asm__ __volatile__("rdtsc \n\t" : "=A" (*(x)))

int main (int argc, const char *argv[])
{
	const char *filename = argv[1];
	const char *target = "elf64-x86-64";

	bfd_init();
	bfd *abfd;
	int frmt;

	void *handle;
	void (*get_sections)(bfd *bfdptr);
	char *err;

	unsigned long long start, finish;

	// 0 = RTLD_LAZY, 1 = RTLD_NOW
	if (!argv[2])
	{
		write(1, "Error: invalid argument. \'0\' -> RTLD_LAZY, \'1\' -> RTLD_NOW\n", 59);
		return 0;
	}
	else if ((*argv[2])-'0' == 0)
	{
		rdtsc(&start);
		handle = dlopen("./lib/libobjdata.so", RTLD_LAZY);
		rdtsc(&finish);
	}
	else if ((*argv[2])-'0' == 1)
	{
		rdtsc(&start);
		handle = dlopen("./lib/libobjdata.so", RTLD_NOW);
		rdtsc(&finish);
	}
	else
	{
		write(1, "Error: invalid argument. \'0\' -> RTLD_LAZY, \'1\' -> RTLD_NOW\n", 59);
		return 0;
	}

	// TEST OUTPUT
	// write_int(finish-start);
	// write(1, "\n", 1);

	get_sections = dlsym(handle, "dump_sections");

	abfd = bfd_openr(filename, target);
	if (abfd == NULL)
	{
		bfd_perror(NULL);
	}

	frmt = bfd_check_format(abfd, bfd_object);
	if (frmt != TRUE)
	{
		bfd_perror(NULL);
	}

	get_sections(abfd);

	dlclose(handle);

	return 0;
}


