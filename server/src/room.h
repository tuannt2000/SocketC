#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <stdbool.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <time.h>

typedef struct
{
	char idRoom[5];
	int totalUserRoom;  // so nguoi choi hien tai trong phong
    int maxUser;    // toi da so nguoi choi trong phong
    int start;     //so nguoi bat dau an start
    int user[3][2];   // nguoi choi va diem
    char name[100]; // ten nguoi choi
    int currentQs;  //cau hoi hien tai
    int playing;    // so nguoi tiep tuc choi
    int userAnswer[3];
    int answered;  //so nguoi da tra loi
    int result[3]; // ket qua dap an(dung,sai)
    int endGame;  //so nguoi da ket thuc tro choi
} Room;

Room * readFileRoom(int *n);

void sendIdRoom(Room *room,int totalRoom,int fd);

int checkRoom(Room *room,int totalRoom,char *idRoom);

int outRoom(Room *room,int totalRoom,char *idRoom);

void start(Room *room,int totalRoom,char * idRoom,char *userName,int fd);

void sendPoint(Room *room,int totalRoom,char *idRoom,int fd);

int indexQs(Room *room,int totalRoom,char *idRoom);

int getCurrentQs(Room *room,int totalRoom,char *idRoom);

void checkAnswer(Room *room,int totalRoom,char *idRoom,char *answer,int fd,char *result);

void endGame(Room *room,int totalRoom,char *idRoom,char *history,bool *check);

int support1(char *sAnswer,char *mess);

int chooseSupport(Room *room,int totalRoom,char *stringSupport,int fd,char *idRoom,char *numberSupport,char *mess);