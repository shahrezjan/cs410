#include <stdio.h>
#include <bfd.h>
#include <objsect.h>

int main (int argc, const char *argv[])
{
	const char *filename = argv[1];
	const char *target = "elf64-x86-64";

	bfd_init();
	bfd *abfd;
	int frmt;

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

	dump_sections(abfd);

	return 0;
}


