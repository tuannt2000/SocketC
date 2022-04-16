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
	char info[2000];
} History;

History * readFileHistory(int *n);

void addHistory(History *history,int *n,char *messHistory);

int writeFileHistory(History *history,int totalHistory);

void historyUser(History *history,int totalHistory,int fd,char *name,char *mess);