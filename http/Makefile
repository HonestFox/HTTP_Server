ROOT_PATH=$(shell pwd)
BIN=httpd
CC=gcc
SRC=httpd.c
OBJ=$(SRC:.c=.o) 						#str change
LIB=-lpthread
FLAGS=#-D_DEBUG_

$(BIN):$(OBJ)
	$(CC) -o $@ $^ $(LIB)
%.o:%.c
	$(CC) -c $< $(FLAGS)				#.c -> .o   in order

.PHONY:clean
clean:
	rm -f *.o $(BIN)


.PHONY:debug
debug:
	@echo $(ROOT_PATH)
	@echo $(BIN)
	@echo $(CC)
	@echo $(SRC)
	@echo $(OBJ)

