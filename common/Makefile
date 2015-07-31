SOURCE = $(wildcard *.c)				# 1. get all .c source files 

OBJS = $(patsubst %.c,%.o,$(SOURCE))	# 2. replace .c with .o by patsubst

LIB = libapue.a
LIB_PATH = ../lib

all: $(LIB)

$(LIB): $(OBJS)
	ar rcs $@ $^
	cp $(LIB) $(LIB_PATH)
	rm -f *.o *.a

$(OBJS): %.o: %.c
	gcc -Wall -g -c -I../include $< -o $@


.PHONY: clean
clean:
	-rm -f *.o *.a