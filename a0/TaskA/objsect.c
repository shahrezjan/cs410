#include <unistd.h>
#include <stdio.h>
#include <bfd.h>

void write_int_as_hex(unsigned long num)
{
	char *hexchars = "0123456789abcdef";

	unsigned long digit = num % 16;
	if (num/16 > 0)
	{
		write_int_as_hex(num/16);
	}
	write(1, hexchars+digit, 1);	
}

void write_int(unsigned long num)
{
	unsigned long digit = num % 10;
	if (num/10 > 0)
	{
		write_int(num/10);
	}
	char digitchar = '0' + digit;
	write(1, &digitchar, 1);
}

void write_buf(const char *str)
{
	int i = 0;
	while(*(str + i) != '\0')
	{
		write(1, str + i, 1);
		++i;
	}
}

void dump_section(bfd *abfd, asection *section, void *obj)
{
	const char comma[] = ", ";

	const char *nm = section->name;
	write_buf(nm);
	write_buf(comma);

	unsigned long vma = section->vma;
	write_int_as_hex(vma);
	write_buf(comma);

	unsigned long rawsize = section->rawsize;
	write_int_as_hex(rawsize);
	write_buf(comma);

	unsigned long cookedsize = section->size;
	write_int_as_hex(cookedsize);
	write_buf(comma);

	unsigned long fileposition = section->filepos;
	write_int_as_hex(fileposition);

	write(1, "\n", sizeof("\n"));
}


void dump_sections(bfd *abfd)
{
	write(1, "Name, VMA, Raw-size, Cooked-size, Position\n", sizeof "Name, VMA, Raw-size, Cooked-size, Position\n");

	bfd_map_over_sections(abfd, dump_section, NULL);

}
