void dump_section(bfd *abfd, asection *section, void *unused);
void dump_sections(bfd *abfd);
void write_buf(const char *str);
void write_int(int num);
void write_int_as_hex(unsigned long num);
