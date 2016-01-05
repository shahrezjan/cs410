CS410 - Assignment 0
Name: Hwa-seung Erstling
BUID: U36770098

Code compilation and testing was performed on BU's linux server: csa2.bu.edu
Target platform used: elf64-x86-64

In folder TaskB:
	Run "make all" to create executables getsyms_dl and getsections_dl
	Run "make clean" to delete executables, object files, and libraries created

	Use getsyms_dl: ./getsyms_dl [binary file] [0 or 1]
		- 0 corresponds to RTLD_LAZY
		- 1 corresponds to RTLD_NOW
		- Ex: ./getsyms_dl objsym.o 1

	Use getsections_dl: ./getsections_dl [binary file] [0 or 1]
		- 0 corresponds to RTLD_LAZY
		- 1 corresponds to RTLD_NOW
		- Ex: ./getsections_dl objsym.o 0

	stats.txt contains the number of cpu clock cyles recorded for dlopen() when called from the two programs above using both RTLD_LAZY and RTLD_NOW. The values were generated using testscript.sh, and copied into stats.txt

In folder TaskA:
	Run "make all" to create executables getsyms and getsections
	Run "make clean" to delete executables, object files, and libraries created

	Use getsyms: ./getsyms [binary file]
		- Ex: ./getsyms objsym.o
	
	Use getsections: ./getsections [binary file]
		- Ex: ./getsections objsym.o
	
		
