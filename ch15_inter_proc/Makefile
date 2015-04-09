BIN_PATH = bin
OBJ_PATH = ../obj

OBJ = ../obj/lock.o \
      ../obj/file_io.o \
      ../obj/err.o \
      ../obj/open_max.o \
      ../obj/my_popen.o \
      ../obj/proc_sync.o

OBJ_OUT = my_popen.o \
          proc_sync.o

# add APPs for different application
APP = add2 \
      invoke_coprocess \
      shm \
      mmap \
      mmap_lock \
      mmap_shm

# needs all, otherwise only one APP is generated
.PHONY: all
all: $(OBJ_OUT) $(APP)

$(OBJ_OUT): %.o: %.c
	gcc -Wall -g -c -I../include $< -o $(OBJ_PATH)/$@
	
$(APP):
	gcc -Wall -g -o $(BIN_PATH)/$@ -I../include $@.c $(OBJ)


.PHONY: clean cleanobj cleanall
cleanall: clean cleanobj

clean:
	-rm -f $(BIN_PATH)/*

cleanobj:
	-rm -f $(OBJ_PATH)/$(OBJ_OUT).o
