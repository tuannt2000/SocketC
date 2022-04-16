#include "src/user.h"

User * readFileUser(int *n) {
    int totalUser = 0;
    User *user;

	/* file reading*/
    user = (User *) calloc(100, sizeof(User));

	FILE* file;
	if ((file = fopen("database/user.txt", "r")) == NULL)
	{
		printf("Error! File cannot be opened.");
		// Program exits if the file pointer returns NULL.
		exit(1);
	}

    char s[100] = {0};

	while(!feof(file))
	{		
        fscanf(file,"%s %s %s",user[totalUser].account,user[totalUser].password,user[totalUser].username);
        user[totalUser].status = 0;
        fgets(s,100,file);

        totalUser++;
	}

    *n = totalUser;

    return user;
}

int writeFileUser(User *user,int totalUser){
    FILE* file;
	if ((file = fopen("database/user.txt", "w")) == NULL)
	{
		printf("Error! File cannot be opened.");
		// Program exits if the file pointer returns NULL.
		exit(1);
	}

	for(int i = 0; i < totalUser ;i++){
        if(i == totalUser - 1) fprintf(file,"%s %s %s",user[i].account,user[i].password,user[i].username);
        else fprintf(file,"%s %s %s\n",user[i].account,user[i].password,user[i].username);
    }

    fclose(file);

    return 0;
}

int checkLogin(User *user,int totalUser,char *account,char *password){
    for(int i = 0; i < totalUser ; i++){
        if(user[i].status == 0){
            if(strcmp(user[i].account,account) == 0){
                if(strcmp(user[i].password,password) == 0){
                    user[i].status = 1;
                    return 1;
                }
            }
        }
    }

    return 0;
}

int getUsername(User *user,int totalUser,char *account,char *username){
    for(int i = 0; i < totalUser ; i++){
        if(strcmp(user[i].account,account) == 0){
            strcpy(username,user[i].username);
            return 1;
        }
    }

    return 0;
}

int logout(User *user,int totalUser,char * username){
    for(int i = 0; i < totalUser ; i++){
        if(strcmp(user[i].username,username) == 0){
            user[i].status = 0;
            return 1;
        }
    }

    return 0;
}

int checkRegister(User *user,int totalUser,char *account,char *password){
    for(int i = 0; i < totalUser ; i++){
        if(strcmp(user[i].account,account) == 0){
            if(strcmp(user[i].password,password) == 0){
                return 1;
            }
        }
    }

    strcpy(user[totalUser].account,account);
    strcpy(user[totalUser].password,password);

    return 0;
}

int checkRegisterUsername(User *user,int *n,char *username){
    int totalUser = *n;

    for(int i = 0; i < totalUser ; i++){
        if(strcmp(user[i].username,username) == 0){
            return 1;
        }
    }

    strcpy(user[totalUser].username,username);
    user[totalUser].status = 0;
    totalUser++;

    *n = totalUser;

    writeFileUser(user,*n);

    return 0;
}