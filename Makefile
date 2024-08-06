CC = gcc
FL_OPT = -Ofast -march=native
FL_DEBUG = -g -Og -Wextra
FL_DEFAULT = -O2

FLAGS = -Wall

ifeq ($(DEBUG), 1)
	FLAGS += $(FL_DEBUG)
endif

ifeq ($(FAST), 1)
	FLAGS += $(FL_OPT)
else
	FLAGS += $(FL_DEFAULT)
endif

ifeq ($(COMPARE), 1)
	FLAGS += -DCOMPARE
endif

all: zoctante test

zoctante: 8080.o machine.o comparison.o
	gcc $(FLAGS) -o zoctante.out main.c i8080.c build/8080.o build/comparison.o build/machine.o -I./raylib/include/ -L./raylib/lib/ -l:libraylib.a -lGL -lm -lpthread -ldl -lrt -lX11

8080.o: 8080.c
	gcc $(FLAGS) -c 8080.c
	mv 8080.o build

machine.o: machine.c 8080.c
	gcc $(FLAGS) -c machine.c -I./raylib/include/ -L./raylib/lib/ -l:libraylib.a -lGL -lm -lpthread -ldl -lrt -lX11
	mv machine.o build

comparison.o: comparison.c
	gcc $(FLAGS) -c comparison.c
	mv comparison.o build

test: test.c machine.o 8080.o
	gcc $(FLAGS) -o test.out test.c build/machine.o build/8080.o -I./raylib/include/ -L./raylib/lib/ -l:libraylib.a -lGL -lm -lpthread -ldl -lrt -lX11

clean:
	rm -f *.o
	rm -f *.out
	rm -f build/*.o
	rm -f build/*.out
