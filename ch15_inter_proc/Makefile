# only change APP for different application
APP = add2
BIN_PATH = bin/
BIN = $(BIN_PATH)$(APP)
OBJ = ../obj/lock.o ../obj/file_io.o ../obj/err.o ../obj/open_max.o ../obj/my_popen.o

$(APP): $(APP).c $(OBJ) ../include/apue.h
	gcc -Wall -g -o $(BIN) -I../include $(APP).c $(OBJ)

.PHONY: clean
clean:
	-rm -f $(BIN_PATH)*
