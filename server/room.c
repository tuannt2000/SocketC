#include "src/room.h"

#define MAX_LINE 2048

Room * readFileRoom(int *n){
    Room *room;
    int totalRoom = 0;

    room = (Room *) calloc(100,sizeof(Room));
    FILE* file;

    if ((file = fopen("database/room.txt", "r")) == NULL)
	{
		printf("Error! File cannot be opened.");
		// Program exits if the file pointer returns NULL.
		exit(1);
	}

    char s[100] = {0};

    while(!feof(file))
	{		
        fscanf(file,"%s %d",room[totalRoom].idRoom,&room[totalRoom].maxUser);
        room[totalRoom].totalUserRoom = 0;
        room[totalRoom].start = 0;
        room[totalRoom].currentQs = 0;
        room[totalRoom].answered = 0;
        room[totalRoom].playing = room[totalRoom].maxUser;
        room[totalRoom].endGame = 0;
        fgets(s,100,file);
            
        totalRoom++;
	}

    *n = totalRoom;

    return room;
}

void sendIdRoom(Room *room,int totalRoom,int fd){
    char buffer[MAX_LINE] = {0};

    for(int i = 0 ; i < totalRoom ; i++){
        strcat(buffer,room[i].idRoom);
        strcat(buffer," ");
    }

    printf("S->C: room:%s\n",buffer);

    send(fd,buffer,strlen(buffer),0);
}

int checkRoom(Room *room,int totalRoom,char *idRoom){
    for(int i = 0; i < totalRoom ; i++){
        if(strcmp(idRoom,room[i].idRoom) == 0){
            room[i].totalUserRoom++;
            if(room[i].totalUserRoom > room[i].maxUser){
                room[i].totalUserRoom--;
                return 1;
            }
        }
    }

    return 0;
}

int outRoom(Room *room,int totalRoom,char *idRoom){
    for(int i = 0; i < totalRoom ; i++){
        if(strcmp(idRoom,room[i].idRoom) == 0){
            room[i].totalUserRoom--;
            return 1;
        }
    }

    return 0;
}

void start(Room *room,int totalRoom,char * idRoom,char *userName,int fd){
    int count;

    for(int i = 0 ; i < totalRoom ; i++){
        if(strcmp(idRoom,room[i].idRoom) == 0){
            count = i;
            room[i].user[room[i].start][0] = fd;
            room[i].user[room[i].start][1] = 0;
            room[i].start++;
            strcat(room[i].name,userName);
            strcat(room[i].name,",");
        }
    }

    if(room[count].start == room[count].maxUser){
        char buffer[MAX_LINE] = {0};
        char start[10] = {0};

        sprintf(start,"%d",room[count].start);

        strcat(buffer,"oke:");
        strcat(buffer,start);
        strcat(buffer," ");
        strcat(buffer,room[count].name);

        for (int i = 0; i < room[count].start; i++)
        {
            printf("S->C: %s\n",buffer);
            send(room[count].user[i][0],buffer,strlen(buffer),0);
        }
    }else{
        char buffer[MAX_LINE] = {0};
        char start[10] = {0};

        sprintf(start,"%d",room[count].start);

        strcat(buffer,"wait:");
        strcat(buffer,start);
        strcat(buffer," ");
        strcat(buffer,room[count].name);

        for (int i = 0; i < room[count].start; i++)
        {
            printf("S->C: %s\n",buffer);
            send(room[count].user[i][0],buffer,strlen(buffer),0);
        }
    }
}

void sendPoint(Room *room,int totalRoom,char *idRoom,int fd){
    for(int i = 0; i < totalRoom ; i++){
        if(strcmp(idRoom,room[i].idRoom) == 0){
            for(int j = 0; j < room[i].maxUser ; j++){
                if(fd == room[i].user[j][0]){
                    char point[10] = {0};

                    sprintf(point,"%d",room[i].user[j][1]);
                    printf("S->C: point:%s\n",point);
                    send(fd,point,strlen(point),0);
                }
            }
        }
    }
}

int indexQs(Room *room,int totalRoom,char *idRoom){
    
    for(int i = 0; i < totalRoom ; i++){
        if(strcmp(idRoom,room[i].idRoom) == 0){
            if(room[i].playing == 1){
                room[i].playing--;
                return -1;
            }else{
                return room[i].currentQs;
            }
        }
    }

    return -2;
}

int getCurrentQs(Room *room,int totalRoom,char *idRoom){
    for(int i = 0; i < totalRoom ; i++){
        if(strcmp(idRoom,room[i].idRoom) == 0){
            return room[i].currentQs;
        }
    }

    return -1;
}

void checkAnswer(Room *room,int totalRoom,char *idRoom,char *answer,int fd,char *result){
    int count;

    for(int i = 0 ; i < totalRoom ; i++){
        if(strcmp(idRoom,room[i].idRoom) == 0){
            count = i;

            room[i].userAnswer[room[i].answered] = fd;

            if(strcmp(answer,"-1") == 0){
                room[i].result[room[i].answered] = -1; 

                for(int j = 0; j < room[i].maxUser ; j++){
                    if(room[i].user[j][0] == fd){
                        room[i].user[j][1] += 0;
                    }
                }
            }else if(strcmp(answer,result) == 0){
                room[i].result[room[i].answered] = 1;

                for(int j = 0; j < room[i].maxUser ; j++){
                    if(room[i].user[j][0] == fd){
                        room[i].user[j][1] += 10;
                    }
                }
            }else{
                room[i].result[room[i].answered] = 0;
            }

            room[i].answered++;
        }
    }

    if(room[count].answered == room[count].playing){
        for(int i = 0 ; i < room[count].answered ; i++){
            char mess[10] = {0};
            char resultUser[5] = {0};

            sprintf(resultUser,"%d",room[count].result[i]);

            strcat(mess,result);
            strcat(mess," ");
            strcat(mess,resultUser);
            printf("S->C: answer:%s\n",mess);
            send(room[count].userAnswer[i],mess,strlen(mess),0);
        }

        for(int i = 0 ; i < room[count].answered ; i++){
            if(room[count].result[i] == 0){
                room[count].playing--;
            }
        }

        room[count].answered = 0;
        room[count].currentQs++;
    }
}

void endGame(Room *room,int totalRoom,char *idRoom,char *history,bool *check){
    char mess[500] = {0};

    for(int i = 0 ; i < totalRoom ; i++){
        if(strcmp(idRoom,room[i].idRoom) == 0){
            room[i].endGame++;

            if(room[i].endGame == room[i].maxUser){
                char *c;
                c = strtok(room[i].name,",");

                for(int j = 0; j < room[i].maxUser ; j++){
                    char point[5] = {0};
                    strcat(mess,c);
                    strcat(mess," ");
                    sprintf(point,"%d",room[i].user[j][1]);
                    strcat(mess,point);
                    strcat(mess,";");
                    c = strtok(NULL,",");
                }
                char messHistory[MAX_LINE] = {0};

                time_t T = time(NULL);
                struct tm tm = *localtime(&T);

                sprintf(messHistory,"%02d/%02d/%04d %02d:%02d:%02d %s",tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900,tm.tm_hour, tm.tm_min, tm.tm_sec,mess);

                strcpy(history,messHistory);

                for(int j = 0 ; j < room[i].start ; j++){
                    printf("S->C: rank:%s\n",mess);
                    send(room[i].user[j][0],mess,strlen(mess),0);
                }

                room[i].name[0] = 0; 
                room[i].currentQs = 0;
                room[i].start = 0;
                room[i].playing = room[i].maxUser;
                room[i].totalUserRoom = 0;
                room[i].endGame = 0;

                *check = true;

                break;
            }else{
                *check = false;

                break;
            }
        }
    }
}

int support1(char *sAnswer,char *mess){
    int answer;
    int answer1 = 0;

    sscanf(sAnswer,"%d",&answer);

    for(int i = 0; i < 2; i++){
        char sRandom[2] = {0};
        int random;

        while (1)
        {
            random = 1 + rand()%4; 

            if(random != answer && random != answer1){
                answer1 = random;
                break;
            }
        }

        sprintf(sRandom,"%d",random);

        if(i == 0){
            strcat(mess,sRandom);
            strcat(mess,",");
        }else{
            strcat(mess,sRandom);
        }

    }
    
    return 0;
}

int chooseSupport(Room *room,int totalRoom,char *stringSupport,int fd,char *idRoom,char *numberSupport,char *mess){
    for(int i = 0 ; i < totalRoom ; i++){
        if(strcmp(idRoom,room[i].idRoom) == 0){
            if(strcmp(numberSupport,"3") == 0){
                strcpy(mess,stringSupport);

                send(fd,mess,strlen(mess),0);

                return 1;
            }else if(strcmp(numberSupport,"2") == 0){
                strcpy(mess,"oke");

                send(fd,mess,strlen(mess),0);

                return 1;
            }else{
                support1(stringSupport,mess);

                send(fd,mess,strlen(mess),0);
                return 1;
            }
        }
    }

    return 0;
}