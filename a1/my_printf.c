#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void write_buf(const char *str);
static void write_int_as_hex(unsigned long num);
static void write_int(unsigned long num);

int my_printf (char *format, ...) 
{
	char **arg = (char **) &format;  //point to format string ptr
	char c;
	static char buf[20];
	int count = 0;
	
	arg++;		//point to first variadic arg

	while ( (c = *format++) != '\0' ) {
		if (c != '%') {
			write(1, &c, 1);
		}
		else {
			char *p;
			c = *format++;		//consume next char
			switch (c)
				{
				case 'd':
					write_int((int)*arg);
					arg++;
					break;
				case 'c':
					//puts("c");
					write(1, &(*arg), 1);
					arg++;
					break;
				case 's':
					write_buf(*arg);
					arg++;
					break;
				case 'x':
					write_int_as_hex((int)*arg);
					arg++;
					break;
				default:
					abort();
				}
		}
	}
  return 0;
}

int my_errprintf (char *format, ...) 
{
	char **arg = (char **) &format;  //point to format string ptr
	char c;
	static char buf[20];
	int count = 0;
	
	arg++;		//point to first variadic arg

	while ( (c = *format++) != '\0' ) {
		if (c != '%') {
			write(2, &c, 1);
		}
		else {
			char *p;
			c = *format++;		//consume next char
			switch (c)
				{
				case 'd':
					write_int((int)*arg);
					arg++;
					break;
				case 'c':
					//puts("c");
					write(2, &(*arg), 1);
					arg++;
					break;
				case 's':
					write_buf(*arg);
					arg++;
					break;
				case 'x':
					write_int_as_hex((int)*arg);
					arg++;
					break;
				default:
					abort();
				}
		}
	}
  return 0;
}


static void write_int_as_hex(unsigned long num)
{
	char *hexchars = "0123456789abcdef";

	unsigned long digit = num % 16;
	if (num/16 > 0)
		write_int_as_hex(num/16);
	write(1, hexchars+digit, 1);	
}

static void write_int(unsigned long num)
{
	unsigned long digit = num % 10;
	if (num/10 > 0)
		write_int(num/10);
	char digitchar = '0' + digit;
	write(1, &digitchar, 1);
}

static void write_buf(const char *str)
{
	int i = 0;
	while(*(str + i) != '\0')
	{
		write(1, str + i, 1);
		++i;
	}
}

