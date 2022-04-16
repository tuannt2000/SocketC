#include "src/question.h"

Quiz * readFileQs(int *n){
    Quiz *quiz_arr;

    int numberQs = 0;
    quiz_arr = (Quiz *) calloc(100,sizeof(Quiz));

    FILE* file;

    char s[100] = {0};
    char s1[10] = {0};
    char s2[150] = {0};

    if ((file = fopen("database/question.txt", "r")) == NULL)
	{
		printf("Error! File cannot be opened.");
		// Program exits if the file pointer returns NULL.
		exit(1);
	}

    while(!feof(file))
	{		
        fgets(s,100,file);
        strcat(quiz_arr[numberQs].question,s);
        fgets(s,100,file);
        strcat(quiz_arr[numberQs].question,s);
        fgets(s,100,file);
        strcat(quiz_arr[numberQs].question,s);
        fgets(s,100,file);
        strcat(quiz_arr[numberQs].question,s);
        fgets(s,100,file);
        strcat(quiz_arr[numberQs].question,s);
        fscanf(file,"%s",s1);
        strcat(quiz_arr[numberQs].answers,s1);
        fgets(s,100,file);
        fgets(s2,150,file);
        strcat(quiz_arr[numberQs].suggest,s2);

        numberQs++;
	}

    *n = numberQs;

    return quiz_arr;
}

void getQs(Quiz *quiz_arr,int index,char *mess,int fd){
    if(index == -1){
        strcpy(mess,"endGame");
        send(fd,mess,strlen(mess),0);
    }else if(index != -2){
        strcpy(mess,quiz_arr[index].question);
        send(fd,quiz_arr[index].question,strlen(quiz_arr[index].question),0);
    }
}

void getResult(Quiz *quiz_arr,int index,char *result){
    strcpy(result,quiz_arr[index].answers);
}

void getSupport(Quiz *quiz_arr,int index,char *numberSupport,char *stringSupport){
    if(strcmp(numberSupport,"1") == 0){
        strcpy(stringSupport,quiz_arr[index].answers);
    }else if(strcmp(numberSupport,"3") == 0){
        strcpy(stringSupport,quiz_arr[index].suggest);
    }
}