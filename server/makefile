CFLAGS = -c -Wall
CC = gcc
LIBS = -lm 

all: server

server: server.o user.o room.o question.o history.o
	${CC} server.o user.o room.o question.o history.o -o server

history.o: history.c
	${CC} ${CFLAGS} history.c

question.o: question.c
	${CC} ${CFLAGS} question.c

room.o: room.c
	${CC} ${CFLAGS} room.c

user.o: user.c
	${CC} ${CFLAGS} user.c

server.o: server.c
	${CC} ${CFLAGS} server.c

clean:
	rm -f *.o *~