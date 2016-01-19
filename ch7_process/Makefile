SOURCE = $(wildcard *.c)
BINS = $(patsubst %.c,%,$(SOURCE))

BIN_PATH = bin
LIB_PATH = -L../lib
LIB	= -lapue -lpthread

# needs all, otherwise only one APP is generated
all: $(BINS)

$(BINS):%:%.c
	gcc -O -Wall -g -o $(BIN_PATH)/$@ -I../include $< $(LIB_PATH) $(LIB)


.PHONY: clean
clean:
	-rm -f $(BIN_PATH)/*
