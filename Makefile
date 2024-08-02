main:
	gcc -g -O0 -o zoctante.out main.c -I./raylib/include/ -L./raylib/lib/ -l:libraylib.a -lGL -lm -lpthread -ldl -lrt -lX11
