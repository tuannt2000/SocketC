#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <arpa/inet.h>

typedef struct
{
	char question[2000];
	char answers[10];
    char suggest[150];
} Quiz;

Quiz * readFileQs(int *n);

void getQs(Quiz *quiz_arr,int index,char *mess,int fd);

void getResult(Quiz *quiz_arr,int index,char *result);

void getSupport(Quiz *quiz_arr,int index,char *numberSupport,char *stringSupport);