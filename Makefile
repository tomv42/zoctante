all: zoctante test

zoctante: 8080.o machine.o comparison.o
	gcc -g -O0 -o zoctante.out main.c i8080.c 8080.o comparison.o machine.o -I./raylib/include/ -L./raylib/lib/ -l:libraylib.a -lGL -lm -lpthread -ldl -lrt -lX11

8080: 8080.c
	gcc -g -O0 -o 8080.o

machine: 8080.o machine.c
	gcc -g -O0 -o machine.o

comparison: comparison.c
	gcc -g -O0 -o comparison.o

test: test.c machine.o 8080.o
	gcc -g -O0 -o test.out test.c machine.o 8080.o
