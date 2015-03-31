OBJ_PATH = ../obj

OBJS = open_max.o

all: $(OBJS)

$(OBJS): %.o: %.c
	gcc -Wall -g -c -I../include $< -o $(OBJ_PATH)/$@

.PHONY: clean
clean:
	-rm -f *.o