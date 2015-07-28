OBJ_PATH = ../obj

OBJS = err.o \
	   lock.o \
	   readn_writen.o \
	   socket.o \
	   daemonize.o \
	   socket_un.o

all: $(OBJS)

$(OBJS): %.o: %.c
	gcc -Wall -g -c -I../include $< -o $(OBJ_PATH)/$@

.PHONY: clean
clean:
	-rm -f $(OBJ_PATH)/*.o