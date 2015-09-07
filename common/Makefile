CC = gcc
CFLAGS = -Wall -g -c

LIB = libapue.a
LIB_PATH = ../lib
INCLUDE = -I../include

SOURCE = $(wildcard *.c)				# 1. get all .c source files 

OBJS = $(patsubst %.c,%.o,$(SOURCE))	# 2. replace .c with .o by patsubst

all: $(LIB)

$(LIB): $(OBJS)
	ar rcs $@ $^
	cp $(LIB) $(LIB_PATH)
	rm -f *.o *.a

$(OBJS): %.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) $< -o $@


.PHONY: clean
clean:
	-rm -f *.o *.a