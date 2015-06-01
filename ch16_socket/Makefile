BIN_PATH = bin
OBJ_PATH = ../obj

OBJ = ../obj/lock.o \
      ../obj/file_io.o \
      ../obj/err.o \
      ../obj/open_max.o \
      ../obj/my_popen.o \
      ../obj/proc_sync.o \
      ../obj/socket.o \
      ../obj/daemonize.o

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
      client_udp \
      uptime_server_multi_port \
      uptime_server_fork_no_wait

# needs all, otherwise only one APP is generated
all: $(APP)

$(APP):%:%.c
	gcc -Wall -g -o $(BIN_PATH)/$@ -I../include $< $(OBJ)


.PHONY: clean
clean:
	-rm -f $(BIN_PATH)/*
