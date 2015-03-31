BIN_PATH = bin
OBJ_PATH = ../obj

OBJ = ../obj/lock.o ../obj/file_io.o ../obj/err.o ../obj/open_max.o ../obj/my_popen.o

MYOPEN = my_popen.o

# add APPs for different application
APP = add2 invoke_coprocess shm

# needs all, otherwise only one APP is generated
.PHONY: all
all: $(MYOPEN) $(APP)

$(MYOPEN): %.o: %.c
	gcc -Wall -g -c -I../include $< -o $(OBJ_PATH)/$@
	
$(APP):
	gcc -Wall -g -o $(BIN_PATH)/$@ -I../include $@.c $(OBJ)


.PHONY: clean cleanobj cleanall
cleanall: clean cleanobj

clean:
	-rm -f $(BIN_PATH)/*

cleanobj:
	-rm -f $(OBJ_PATH)/$(MYOPEN).o
