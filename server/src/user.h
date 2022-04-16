#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>

typedef struct
{
	char account[20];
	char password[10];
    char username[20];
    int status;
} User;

User * readFileUser();

int writeFileUser(User *user,int totalUser);

int checkLogin(User *user,int totalUser,char *account,char *password);

int getUsername(User *user,int totalUser,char *account,char *username);

int logout(User *user,int totalUser,char * username);

int checkRegister(User *user,int totalUser,char *account,char *password);

int checkRegisterUsername(User *user,int *n,char *username);