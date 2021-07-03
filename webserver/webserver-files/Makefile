#
# To compile, type "make" or make "all"
# To remove files, type "make clean"
#
OBJS = server.o request.o Wrappers.o client.o PQueue.o
TARGET = server

CC = gcc
CFLAGS = -g -Wall

LIBS = -lpthread 

.SUFFIXES: .c .o 

all: server client output.cgi
	-mkdir -p public
	-cp output.cgi home.html public

server: server.o request.o Wrappers.o PQueue.o
	$(CC) $(CFLAGS) -o server server.o request.o Wrappers.o PQueue.o $(LIBS)

client: client.o Wrappers.o
	$(CC) $(CFLAGS) -o client client.o Wrappers.o

output.cgi: output.c
	$(CC) $(CFLAGS) -o output.cgi output.c

.c.o:
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	-rm -f $(OBJS) server client output.cgi
	-rm -rf public
