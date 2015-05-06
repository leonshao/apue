BIN_PATH = bin
OBJ_PATH = ../obj

OBJ = ../obj/lock.o \
      ../obj/file_io.o \
      ../obj/err.o \
      ../obj/open_max.o \
      ../obj/my_popen.o \
      ../obj/proc_sync.o \
      ../obj/socket.o

# add APPs for different application
APP = endian \
      socket_stat \
      gethostname \
      gethostent \
      getprotoent \
      getservent \
      getaddrinfo \
      uptime_server \
      client \
      uptime_server_fork \
      uptime_server_udp \
      client_udp

# needs all, otherwise only one APP is generated
all: $(APP)

$(APP):
	gcc -Wall -g -o $(BIN_PATH)/$@ -I../include $@.c $(OBJ)


.PHONY: clean
clean:
	-rm -f $(BIN_PATH)/*
