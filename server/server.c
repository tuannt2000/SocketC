#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <pwd.h>
#include <sys/stat.h>
#include <syslog.h>
#include <time.h>
#include <arpa/inet.h>
#include <errno.h>

#include "src/room.h"
#include "src/user.h"
#include "src/question.h"
#include "src/history.h"

#define PORT 8080
#define MAXCN 10
#define MAX_LINE 2048

int nread;
int listenSocket = 0;
fd_set master;

int totalUser = 0;
int totalRoom = 0;
int numberQs = 0;
int totalHistory = 0;

User *user = NULL;
Room *room = NULL;
Quiz *quiz_arr = NULL;
History *history = NULL;

void freeData() {
    free(user);
    free(room);
    free(quiz_arr);
    free(history);
}

void handleDataFromClient(int fd){
    char buffer[MAX_LINE] = {0};
    nread = recv(fd,buffer,1000,0);
    buffer[nread] = 0;

    if(nread <= 0) return;

    printf("%s\n",buffer);

    char *c;
    c = strtok(buffer,":");

    if(strcmp(c,"login") == 0){
        char account[20] = {0};
        char password[10] = {0};
        char mess[MAX_LINE] = {0};
        char *c;
        c = strtok(NULL," ");
        strcpy(account,c);
        c = strtok(NULL," ");
        strcpy(password,c);

        if(checkLogin(user,totalUser,account,password) == 1){
            char username[20];
            strcat(mess,"login:1");
            printf("S->C: %s\n",mess);
            send(fd,mess,strlen(mess),0);
            nread = recv(fd,buffer,100,0);
            buffer[nread] = 0;
            printf("%s\n",buffer);
            if(getUsername(user,totalUser,account,username) == 1){
                strcpy(mess,"username:");
                strcat(mess,username);
                printf("S->C: %s\n",mess);
                send(fd,mess,strlen(mess),0);
            }

            nread = recv(fd,buffer,100,0);
            buffer[nread] = 0;
            printf("%s\n",buffer);
            sendIdRoom(room,totalRoom,fd);
        }else{
            strcat(mess,"login:0");
            printf("S->C: %s\n",mess);
            send(fd,mess,strlen(mess),0);
        }
    }else if(strcmp(c,"logout") == 0){
        char mess[MAX_LINE] = {0};
        char username[20] = {0};

        c = strtok(NULL," ");
        strcpy(username,c);

        if(logout(user,totalUser,username) == 1){
            strcpy(mess,"logout:1");
            printf("S->C: %s\n",mess);
            send(fd,mess,strlen(mess),0);
        }else{
            strcpy(mess,"logout:0");
            printf("S->C: %s\n",mess);
            send(fd,mess,strlen(mess),0);
        }
    }else if(strcmp(c,"register") == 0){
        char mess[MAX_LINE] = {0};
        char account[10] = {0};
        char password[10] = {0};
        c = strtok(NULL," ");
        strcpy(account,c);
        c = strtok(NULL," ");
        strcpy(password,c);
        if(checkRegister(user,totalUser,account,password) == 0){
            strcpy(mess,"register:1");
            printf("S->C: %s\n",mess);
            send(fd,mess,strlen(mess),0);

            while (1)
            {
                char username[20] = {0};
                nread = recv(fd,buffer,100,0);
                buffer[nread] = 0;
                printf("%s\n",buffer);

                c = strtok(buffer,":");
                c = strtok(NULL," ");
                strcpy(username,c);

                if(checkRegisterUsername(user,&totalUser,username) == 0){
                    strcpy(mess,"registerUsername:1");
                    printf("S->C: %s\n",mess);
                    send(fd,mess,strlen(mess),0);
                    break;
                }else{
                    strcpy(mess,"registerUsername:0");
                    printf("S->C: %s\n",mess);
                    send(fd,mess,strlen(mess),0);
                }
            }
            
        }else{
            strcpy(mess,"register:0");
            printf("S->C: %s\n",mess);
            send(fd,mess,strlen(mess),0);
        }
    }else if(strcmp(c,"idRoom") == 0){
        char mess[MAX_LINE] = {0};
        char idRoom[5] = {0};
        char name[20] = {0};
        c = strtok(NULL," ");
        strcpy(idRoom,c);
        c = strtok(NULL," ");
        strcpy(name,c);

        if (checkRoom(room,totalRoom,idRoom) == 1)
        {
            strcpy(mess,"idRoom:");
            strcat(mess,idRoom);
            strcat(mess," 0");
            printf("S->C: %s\n",mess);
            send(fd,mess,strlen(mess),0);
        }else{
            strcpy(mess,"idRoom:");
            strcat(mess,idRoom);
            strcat(mess," 1");
            printf("S->C: %s\n",mess);
            send(fd,mess,strlen(mess),0);
        }
    }else if(strcmp(c,"outRoom") == 0){
        char mess[MAX_LINE] = {0};
        char idRoom[10] = {0};
        c = strtok(NULL," ");
        strcpy(idRoom,c);
        if(outRoom(room,totalRoom,idRoom) == 1){
            strcpy(mess,"outRoom:oke");
            printf("S->C: %s\n",mess);
        }else{
            strcpy(mess,"outRoom:0");
            printf("S->C: %s\n",mess);
        }
    }else if(strcmp(c,"start") == 0){
        char idRoom[5] = {0};
        char userName[20] = {0};

        c = strtok(NULL," ");
        strcpy(idRoom,c);
        c = strtok(NULL," ");
        strcpy(userName,c);
        
        start(room,totalRoom,idRoom,userName,fd);
    }else if(strcmp(c,"getQs") == 0){
        char mess[MAX_LINE] = {0};
        char idRoom[5] = {0};

        c = strtok(NULL," ");
        strcpy(idRoom,c);

        int index = indexQs(room,totalRoom,idRoom);

        getQs(quiz_arr,index,mess,fd);

        printf("S->C: getQs:%s\n",mess);
    }else if(strcmp(c,"point") == 0){
        char idRoom[5] = {0};

        c = strtok(NULL," ");
        strcpy(idRoom,c);

        sendPoint(room,totalRoom,idRoom,fd);
    }else if(strcmp(c,"answer") == 0){
        char idRoom[5] = {0};
        char answer[5] = {0};

        char result[5] = {0};   // dap an cau hoi hien tai

        c = strtok(NULL," ");
        strcpy(idRoom,c);
        c = strtok(NULL," ");
        strcpy(answer,c);

        int index = getCurrentQs(room,totalRoom,idRoom);

        getResult(quiz_arr,index,result);

        checkAnswer(room,totalRoom,idRoom,answer,fd,result);
    }else if(strcmp(c,"endGame") == 0){
        char idRoom[5] = {0};
        char messHistory[2000] = {0};

        bool check = false;

        c = strtok(NULL," ");
        strcpy(idRoom,c);

        endGame(room,totalRoom,idRoom,messHistory,&check);

        if(check){
            addHistory(history,&totalHistory,messHistory);

            writeFileHistory(history,totalHistory);
        }
    }else if(strcmp(c,"support") == 0){
        char mess[MAX_LINE] = {0};
        char idRoom[5] = {0};
        char numberSupport[5] = {0};
        char stringSupport[MAX_LINE] = {0};

        c = strtok(NULL," ");
        strcpy(idRoom,c);

        c = strtok(NULL," ");
        strcpy(numberSupport,c);

        int index = getCurrentQs(room,totalRoom,idRoom);

        getSupport(quiz_arr,index,numberSupport,stringSupport);

        chooseSupport(room,totalRoom,stringSupport,fd,idRoom,numberSupport,mess);

        printf("S->C: support:%s %s\n",numberSupport,mess);
    }else if(strcmp(c,"history") == 0){
        char mess[MAX_LINE] = {0};
        char name[10] = {0};

        c = strtok(NULL," ");
        strcpy(name,c);

        historyUser(history,totalHistory,fd,name,mess);
    }
}

int main(int argc, char const *argv[])  
{ 
    socklen_t sin_size;
    int player;
	struct sockaddr_in server,client_addr;
    int opt = 1; 
    int fdmax, i, rc;
    fd_set read_fds;
    FD_ZERO(&master);
    FD_ZERO(&read_fds);
    //tao socket
    // tao file mo ta soket
    if ((listenSocket = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
    { 
        perror("socket failed"); 
        exit(EXIT_FAILURE); 
    }  
    //gan dia chi cho socket
    // gan cong port 8080 
    if (setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) 
    { 
        perror("setsockopt"); 
        exit(EXIT_FAILURE); 
    } 
    server.sin_family = AF_INET; 
    server.sin_addr.s_addr = INADDR_ANY; 
    server.sin_port = htons( PORT );  //gan cong la 8080   
    // bind 
    if (bind(listenSocket, (struct sockaddr *)&server, sizeof(server))<0) 
    { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    }
    //listen, chi dinh socket lang nghe ket noi
    if (listen(listenSocket, MAXCN) < 0) 
    { 
        perror("listen"); 
        exit(EXIT_FAILURE); 
    } 

    FD_SET(listenSocket, &master);
    fdmax = listenSocket;

    // Set timeout
    struct timeval timeout;
    timeout.tv_sec  = 2000;  // after 5 seconds will timeout
    timeout.tv_usec = 0;
    //Step 4: Communicate with clients

    user =  readFileUser(&totalUser);
    room = readFileRoom(&totalRoom);
    quiz_arr = readFileQs(&numberQs);
    history = readFileHistory(&totalHistory);

    while(1){
        FD_ZERO(&read_fds);
        read_fds = master;
        rc = select(fdmax + 1, &read_fds, NULL, NULL, &timeout);
        if( rc == -1){
            perror("select() error!\n");
            exit(-6);
        }
        else if (rc == 0){
        // printf("select() timed out. End program.\n");
        // exit(-5);
        }
        for(i = 0; i <= fdmax; i++){
            if(FD_ISSET(i, &read_fds)){
                if(i == listenSocket){
                    sin_size = sizeof(struct sockaddr_in);
                    player = accept(listenSocket, (struct sockaddr*)&client_addr, &sin_size);
                    if(player == -1){
                        perror("accept error!\n");
                        exit(-7);
                    }
                    else{
                        FD_SET(player, &master);
                        if(player > fdmax)
                        fdmax = player;
                        printf("Got a connection from (%s , %d) with fd = %d\n", inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port), player);
                    }
                }
                else{
                    handleDataFromClient(i);
                }
            }
        }
    }

    close(listenSocket);
    freeData();
    return 0; 
}