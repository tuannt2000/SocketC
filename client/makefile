CFLAGS = -c -Wall
CC = gcc
LIBS = -lm 

all: client

client: client.o 
	${CC} client.o -o client 

client.o: client.c 
	${CC} ${CFLAGS} client.c

clean:
	rm -f *.o *~