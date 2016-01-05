#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <bfd.h>

void write_int_as_hex1(unsigned long num)
{
	char *hexchars = "0123456789abcdef";

	unsigned long digit = num % 16;
	if (num/16 > 0)
	{
		write_int_as_hex1(num/16);
	}
	write(1, hexchars+digit, 1);	
}

void write_int1(unsigned long num)
{
	unsigned long digit = num % 10;
	if (num/10 > 0)
	{
		write_int1(num/10);
	}
	char digitchar = '0' + digit;
	write(1, &digitchar, 1);
}


void write_buf1(const char *str)
{
	int i = 0;
	while(*(str + i) != '\0')
	{
		write(1, str + i, 1);
		++i;
	}
}

void dump_symbols(bfd *abfd)
{

	long storage_needed;
	asymbol **symbol_table;
	long number_of_symbols;
	long i;

	storage_needed = bfd_get_symtab_upper_bound (abfd);

	if (storage_needed < 0)
		return;

	if (storage_needed == 0)
		return;

	symbol_table = malloc (storage_needed);
	
	number_of_symbols = bfd_canonicalize_symtab (abfd, symbol_table);

	if (number_of_symbols < 0)
		return;

	write(1, "Symbol name, VMA\n", sizeof "Symbol name, VMA\n");

	for (i = 0; i < number_of_symbols; i++)
	{
		write_buf1(symbol_table[i]->name);
		write(1, ", ", sizeof(", "));

		write_int_as_hex1(symbol_table[i]->section->vma+symbol_table[i]->value);
		write(1, "\n", 1);

	}
}
