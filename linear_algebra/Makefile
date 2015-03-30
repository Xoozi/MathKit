VPATH = src:include

BIN_DIR = bin

INCLUDE_DIR = include

objects = main.o matrix.o

all: $(objects)
	gcc -o $(BIN_DIR)/matkit $(objects)
	rm *.o

main.o: main.c matrix.h
	gcc -pg -g -c $< -I$(INCLUDE_DIR)

matrix.o: matrix.c matrix.h
	gcc -pg -g -c $< -I$(INCLUDE_DIR)
