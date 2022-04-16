#include "src/history.h"

History * readFileHistory(int *n){
    History *history;
    history = (History *) calloc(100,sizeof(History));

    int totalHistory = 0;

    FILE* file;

    char s[100] = {0};

    if ((file = fopen("database/history.txt", "r")) == NULL)
	{
		printf("Error! File cannot be opened.");
		// Program exits if the file pointer returns NULL.
		exit(1);
	}

    while(!feof(file))
	{	
        fgets(s,100,file);
        strcpy(history[totalHistory].info,s);

        totalHistory++;
	}

    *n = totalHistory;

    return history;
}

void addHistory(History *history,int *n,char *messHistory){
    int totalHistory = *n;

    strcpy(history[totalHistory].info,messHistory);

    totalHistory++;

    *n = totalHistory;
}

int writeFileHistory(History *history,int totalHistory){
    FILE* file;
	if ((file = fopen("database/history.txt", "w")) == NULL)
	{
		printf("Error! File cannot be opened.");
		// Program exits if the file pointer returns NULL.
		exit(1);
	}

	for(int i = 0; i < totalHistory ;i++){
        if(i == totalHistory - 1) fprintf(file,"%s",history[i].info);
        else fprintf(file,"%s\n",history[i].info);
    }

    fclose(file);

    return 0;
}

void historyUser(History *history,int totalHistory,int fd,char *name,char *mess){
    for(int i = 0; i < totalHistory; i++){
        if(strstr(history[i].info,name) != NULL){
            strcat(mess,history[i].info);
            strcat(mess,"\n");
        }
    }

    if(strlen(mess) == 0){
        strcat(mess,"0");
    }

    printf("S->C: history:%s\n",mess);
    send(fd,mess,strlen(mess),0);
}