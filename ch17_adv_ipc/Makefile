BIN_PATH = bin
OBJ_PATH = ../obj

OBJ = ../obj/err.o \
	  ../obj/socket_un.o

# add APPs for different application
APP = bind_un \
	  server_un \
	  client_un

# needs all, otherwise only one APP is generated
all: $(APP)

$(APP):%:%.c
	gcc -Wall -g -o $(BIN_PATH)/$@ -I../include $< $(OBJ)


.PHONY: clean
clean:
	-rm -f $(BIN_PATH)/*
