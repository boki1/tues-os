all:
	nasm -g -f elf64 plat.s
	gcc -ggdb3 main.cpp plat.o

clean:
	rm -f *.o a.out

.PHONY: all clean
