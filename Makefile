opt = -O3


all: zoctante test

zoctante: 8080.o machine.o comparison.o
	gcc -g $(opt) -o zoctante.out main.c i8080.c 8080.o comparison.o machine.o -I./raylib/include/ -L./raylib/lib/ -l:libraylib.a -lGL -lm -lpthread -ldl -lrt -lX11

8080: 8080.c
	gcc -g $(opt) -o 8080.o 8080.c

machine: machine.c 8080.o
	gcc -g $(opt) -o machine.o machine.c 8080.o -I./raylib/include/ -L./raylib/lib/ -l:libraylib.a -lGL -lm -lpthread -ldl -lrt -lX11

comparison: comparison.c
	gcc -g $(opt) -o comparison.o comparison.c

test: test.c machine.o 8080.o
	gcc -g $(opt) -o test.out test.c machine.o 8080.o -I./raylib/include/ -L./raylib/lib/ -l:libraylib.a -lGL -lm -lpthread -ldl -lrt -lX11

clean:
	rm *.o
	rm *.out
