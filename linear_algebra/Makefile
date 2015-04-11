VPATH = src:include

BIN_DIR = bin

INCLUDE_DIR = include

objects = main.o matrix.o cmd.o handler.o table.o object.o type.o cson.o

all: $(objects)
	gcc -o  $(BIN_DIR)/matkit $(objects) -lm
	rm *.o

main.o: main.c matrix.h handler.h
	gcc -pg -g -c $< -I$(INCLUDE_DIR)


type.o: type.c type.h 
	gcc -pg -g -c $< -I$(INCLUDE_DIR)

object.o: object.c object.h type.h
	gcc -pg -g -c $< -I$(INCLUDE_DIR)

table.o: table.c table.h
	gcc -pg -g -c $< -I$(INCLUDE_DIR)

matrix.o: matrix.c matrix.h
	gcc -pg -g -c $< -I$(INCLUDE_DIR)

cmd.o: cmd.c cmd.h
	gcc -pg -g -c $< -I$(INCLUDE_DIR)

cson.o: cson.c cson.h
	gcc  -pg -g -c  $< -I$(INCLUDE_DIR)

handler.o: handler.c handler.h cmd.h matrix.h table.h cson.h
	gcc -pg -g -c $< -I$(INCLUDE_DIR)
