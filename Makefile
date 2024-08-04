main: 8080.o
	gcc -g -O0 -o zoctante.out main.c i8080.c 8080.o -I./raylib/include/ -L./raylib/lib/ -l:libraylib.a -lGL -lm -lpthread -ldl -lrt -lX11

8080:
	gcc -g -O0 -o 8080.o
