opt = -O0


all: zoctante test

zoctante: 8080.o machine.o comparison.o
	gcc -g $(opt) -o zoctante.out main.c i8080.c 8080.o comparison.o machine.o -I./raylib/include/ -L./raylib/lib/ -l:libraylib.a -lGL -lm -lpthread -ldl -lrt -lX11

8080: 8080.c
	gcc -g $(opt) -o 8080.o

machine: 8080.o machine.c
	gcc -g $(opt) -o machine.o

comparison: comparison.c
	gcc -g $(opt) -o comparison.o

test: test.c machine.o 8080.o
	gcc -g $(opt) -o test.out test.c machine.o 8080.o

clean:
	rm *.o
	rm *.out
