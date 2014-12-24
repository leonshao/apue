OBJ = ../obj/lock.o ../obj/file_io.o ../obj/err.o
#dead_lock: dead_lock.c ../obj/lock.o ../obj/err.o ../include/apue.h
#	gcc -Wall -o dead_lock -I../include dead_lock.c ../obj/lock.o ../obj/err.o 

#nonblk_write: nonblk_write.c ../obj/file_io.o ../obj/err.o ../include/apue.h ../include/file_io.h
#	gcc -Wall -o nonblk_write -I../include nonblk_write.c ../obj/file_io.o ../obj/err.o

#mandatory_lock: mandatory_lock.c $(OBJ) ../include/apue.h
#	gcc -Wall -o mandatory_lock -I../include mandatory_lock.c $(OBJ)

#mod_list: mod_list.c $(OBJ) ../include/apue.h
#	gcc -Wall -o mod_list -I../include mod_list.c $(OBJ)

#cp_mmap: cp_mmap.c $(OBJ) ../include/apue.h
#	gcc -Wall -o cp_mmap -I../include cp_mmap.c $(OBJ)

lock_starve: lock_starve.c $(OBJ) ../include/apue.h
	gcc -Wall -g -o lock_starve -I../include lock_starve.c $(OBJ)

.PHONY: clean
clean:
	-rm -f dead_lock nonblk_write mandatory_lock mod_list cp_mmap lock_starve
