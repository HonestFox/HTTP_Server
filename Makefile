ROOT_PATH=$(shell pwd)
BIN=httpd
CC=gcc
SRC=httpd.c
OBJ=$(SRC:.c=.o) 						#str change
LIB=-lpthread
FLAGS=#-D_DEBUG_
CGI=$(ROOT_PATH)/cgi $(ROOT_PATH)/mysql_connect

$(BIN):$(OBJ)
	$(CC) -o $@ $^ $(LIB)
%.o:%.c
	$(CC) -c $< $(FLAGS) $(LIB)				#.c -> .o   in order

.PHONY:all
all:
	$(BIN) cgi
$(BIN):$(OBj)
	gcc -o $@ $^
$(OBJ):$(SRC)
	gcc -o $@ $^

.PHONY:cgi
cgi:
	for name in `echo $(CGI)`:\
		do\
			cd $$name;\
			make\
			cd -;\
		done


.PHONY:clean
clean:
	rm -rf *.o $(BIN) output
	for name in `echo $(CGI)`:\
		do\
			cd $$name;\
			make clean\
			cd -;\
		done


.PHONY:debug
debug:
	@echo $(ROOT_PATH)
	@echo $(BIN)
	@echo $(CC)
	@echo $(SRC)
	@echo $(OBJ)

.PHONY:output
output:
	mkdir -p output/lib
	mkdir -p output/cgi
	mkdir -p output/htdoc/imag
	cp -f httpd output
	cp htdoc/cgi/cgi_math output/htdoc/cgi
	cp htdoc/imag/* output/htdoc/imag
	cp htdoc/index.html output/htdoc
	cp mysql_connect/cgi_insert output/htdoc/cgi
	cp mysql_connect/cgi_select output/htdoc/cgi
	cp start.sh output
	cp lib/lib/* output/lib
