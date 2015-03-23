OBJ = ../obj/lock.o ../obj/file_io.o ../obj/err.o

#pipe: pipe.c $(OBJ) ../include/apue.h
#	gcc -Wall -g -o pipe -I../include pipe.c $(OBJ)

pager: pager.c $(OBJ) ../include/apue.h
	gcc -Wall -g -o pager -I../include pager.c $(OBJ)

#popen: popen.c $(OBJ) ../include/apue.h
#	gcc -Wall -g -o popen -I../include popen.c $(OBJ)


.PHONY: clean
clean:
	-rm -f pipe pager popen
