#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <termios.h>
#include <stdbool.h>

#define PORT 8080
#define MAX_LINE 2048

int sock;
int nread;

int totalSp = 0;

int totalRoom = 0;

int totalRank = 0;

typedef struct 
{
    int numberSp; 
    int checkSp;
}Support;

typedef struct 
{
    char idRoom[5];
}Room;

typedef struct 
{
    int point;   
    char name[50];
}Rank;

Support *support = NULL;

Room *room = NULL;

Rank *rank = NULL;

char name[10] = {0};
char maRoom[10] = {0};

void menu();
void getRoom();
void prepare();

void setSupport(){
    support = NULL;
    totalSp = 3;
    support = (Support *) calloc(3,sizeof(Support));

    for (int i = 0; i < totalSp; i++)
    {
        support[i].numberSp = i + 1;
        support[i].checkSp = 1;
    }  
}

/* reads from keypress, doesn't echo */
int getch(void){
    struct termios oldattr, newattr;
    int ch;
    tcgetattr( STDIN_FILENO, &oldattr );
    newattr = oldattr;
    newattr.c_lflag &= ~( ICANON | ECHO );
    tcsetattr( STDIN_FILENO, TCSANOW, &newattr );
    ch = getchar();
    tcsetattr( STDIN_FILENO, TCSANOW, &oldattr );
    return ch;
}

void gotoxy(int x,int y){
    printf("%c[%d;%df",0x1B,y,x);
}

void rankUser(char *buffer){
    char *c;
    rank = NULL;
    totalRank = 0;
    rank = (Rank *) calloc(100,sizeof(Rank));

    c = strtok(buffer,";");
    while (c != NULL)
    {
        char mess[MAX_LINE] = {0};
        strcpy(mess,c);
        sscanf(mess,"%s %d",rank[totalRank].name,&rank[totalRank].point);
        c = strtok(NULL,";");
        totalRank++;
    }

    int top = 1;

    while (totalRank > 0)
    {
        int max = 0;

        for(int i = 0; i < totalRank ; i++){
            if(rank[i].point > max){
                max = rank[i].point;
            }
        }

        printf("\nTop %d : ",top);

        for(int i = 0; i < totalRank ; i++){
            if(rank[i].point == max){
                printf("%s %d;",rank[i].name,rank[i].point);

                for(int j = i; j < totalRank - 1 ; j++){
                    rank[j] = rank[j+1];
                }

                totalRank--;
                i--;
            }
        }

        top++;
    }
}

void endGame(){
    char buffer[MAX_LINE] = {0};
    char mess[MAX_LINE] = {0};
    char input;

    strcat(mess,"endGame:");
    strcat(mess,maRoom);

    send(sock,mess,strlen(mess),0);
    nread = recv(sock,buffer,MAX_LINE,0);
    buffer[nread] = 0;

    sleep(2);
    printf("Tro choi ket thuc\n");
    printf("Bang thanh tich cua ban\n");
    sleep(2);

    rankUser(buffer);

    printf("\nAn Enter de thoat");

    input = getch();

    if(input == 10){
        getRoom();
    }
}

void chooseSupport(int *count,char *messSupport){
    *count = 1;
    char input;

    while (1)
    {
        system("clear");
        printf("Hay chon su tro giup cua ban\n");
        printf("1.50-50\n");
        printf("2.Bo qua cau hoi nay\n");
        printf("3.Goi y tu chuong trinh\n");
        printf("Ban chon : %d\n",*count);

        input = getch();

        if(input == 27){
            input = getch();
            if(input == 91){
                input = getch();
                if(input == 65){
                    *count = *count - 1;
                    if(*count < 1){
                        *count = 3;
                    }
                }else if(input == 66){
                    *count = *count + 1;
                    if(*count > 3){
                        *count = 1;
                    }
                }
            }
        }else if(input == 10){
            if(support[*count - 1].checkSp == 1){
                if(*count == 1){
                printf("Ban da chon su tro giup 50-50\n");
                sleep(1);
                    printf("Chuong trinh se bo di 2 phuong an sai\n");
                    sleep(1);
                }else if(*count == 2){
                    printf("Ban da chon bo qua cau hoi nay\n");
                    sleep(1);
                    printf("Ban se buoc sang cau hoi ke tiep\n");
                    printf("nhung se khong co diem\n");
                    sleep(1);
                }else if(*count == 3){
                    printf("Ban da chon su tro giup goi y\n");
                    sleep(1);
                    printf("Chuong trinh se dua ra goi y cho ban\n");
                    sleep(1);
                }

                printf("An ENTER de quay lai\n");

                input = getch();

                while (1)
                {
                    if(input == 10){
                        break;
                    }
                }
                

                support[*count - 1].checkSp = 0;

                char mess[MAX_LINE] = {0};
                char numberSupport[10] = {0};

                sprintf(numberSupport,"%d",*count);
                strcat(mess,"support:");
                strcat(mess,maRoom);
                strcat(mess," ");
                strcat(mess,numberSupport);
                send(sock,mess,strlen(mess),0);
                nread = recv(sock,messSupport,MAX_LINE,0);
                messSupport[nread] = 0;


                break;
            }else{
                printf("Ban da su dung su tro giup nay\n");
                sleep(1);
            }
        }
    }
    
}

void chooseAnswer(int count,bool *check){
    char buffer[MAX_LINE] = {0};
    char mess[MAX_LINE] = {0};
    char answer[10] = {0};

    sprintf(answer,"%d",count);
    strcat(mess,"answer:");
    strcat(mess,maRoom);
    strcat(mess," ");
    strcat(mess,answer);
    send(sock,mess,strlen(mess),0);

    printf("Cho nhung nguoi choi khac tra loi\n");

    nread = recv(sock,buffer,10,0);
    buffer[nread] = 0;

    printf("Moi nguoi da tra loi\n");
    sleep(2);
    printf("Ket qua cua chuong trinh la ");
    sleep(2);

    char *c;
    c = strtok(buffer," ");
    printf("%s\n",c);
    c = strtok(NULL," ");

    if(strcmp(c,"1") == 0){
        printf("\033[0;32m");
        printf("Chinh xac\n");
        printf("\033[0m");
        sleep(1);
        printf("Chuan bi cau hoi ke tiep\n");
    }else if(strcmp(c,"0") == 0){
        printf("\033[0;31m");
        printf("Ban da tra loi sai\n");
        printf("Ban bi loai\n");
        printf("\033[0m");
        *check = false;
    }else{
        printf("\033[0;33m");
        printf("Ban khong duoc cong diem o vong nay\n");
        printf("\033[0m");
        sleep(1);
        printf("Chuan bi cau hoi ke tiep\n");
    }

    char input;

    printf("An phim ENTER de tiep tuc\n");

    while (1)
    {
        input = getch();

        if(input == 10){
            break;
        }
    }
    
}

void playgame(){
    bool check = true;

    while (check)
    {
        char buffer[MAX_LINE] = {0};
        char messSupport[MAX_LINE] = {0};
        char messSupport1[MAX_LINE] = {0};
        char messSupport3[MAX_LINE] = {0};
        char input;
        int support = 0;
        char point[10] = {0};
        
        strcat(point,"point:");
        strcat(point,maRoom);
        send(sock,point,strlen(point),0);
        nread = recv(sock,point,10,0);
        point[nread] = 0;

        char play[20] = {0};
        strcat(play,"getQs:");
        strcat(play,maRoom);
        send(sock,play,strlen(play),0);
        int count = 1;
        nread = recv(sock,buffer,MAX_LINE,0);
        buffer[nread] = 0;

        if(strcmp(buffer,"endGame") == 0){
            break;
        }

        while (1)
        {
            system("clear");
            printf("%s",buffer);
            printf("An 'o' de su dung su tro giup\n");
            printf("Diem hien tai cua ban la : %s\n\n",point);

            if(support == 1){
                strcpy(messSupport1,messSupport);
            }else if(support == 3){
                strcpy(messSupport3,messSupport);
            }

            if(strcmp(messSupport1,"") != 0){
                printf("Chuong trinh dua ra 2 phuong an sai : %s\n",messSupport1);
            }

            if(strcmp(messSupport3,"") != 0){
                printf("Goi y tu chuong trinh : %s\n",messSupport3);
            }

            if(support == 2){
                count = -1;
                chooseAnswer(count,&check);

                break;
            }

            printf("Ban chon:%d\n",count);

            input = getch();

            if(input == 27){
                input = getch();
                if(input == 91){
                    input = getch();
                    if(input == 65){
                        count--;
                        if(count < 1){
                            count = 4;
                        }
                    }else if(input == 66){
                        count++;
                        if(count > 4){
                            count = 1;
                        }
                    }
                }
            }else if(input == 10){

                chooseAnswer(count,&check);

                break;
            }else if(input == 111){

                chooseSupport(&support,messSupport);

            }
        }   
    }

    system("clear");
    printf("Cho moi nguoi ket thuc tro choi\n");
    endGame();
}

void start(){
    char buffer[MAX_LINE] = {0};

    while (1)
    {
        char *c;
        nread = recv(sock,buffer,MAX_LINE,0);
        buffer[nread] = 0;
        
        c = strtok(buffer,":");

        system("clear");

        if(strcmp(c,"wait") == 0){
            char numberUser[10] = {0};
            char nameUser[MAX_LINE] = {0};

            c = strtok(NULL," ");
            strcpy(numberUser,c);
            c = strtok(NULL," ");
            strcpy(nameUser,c);

            gotoxy(0,0);
            printf("Nick name : %s\n",name);
            gotoxy(20,2);
            printf("Cho du so luong nguoi choi...\n");
            gotoxy(20,3);
            printf("Dang co %s nguoi trong phong\n",numberUser);
            gotoxy(20,4);
            printf("Danh sach ten nguoi choi : %s\n",nameUser);
        }else if(strcmp(c,"oke") == 0){
            char numberUser[10] = {0};
            char nameUser[MAX_LINE] = {0};

            c = strtok(NULL," ");
            strcpy(numberUser,c);
            c = strtok(NULL," ");
            strcpy(nameUser,c);

            gotoxy(0,0);
            printf("Nick name : %s\n",name);
            gotoxy(20,2);
            printf("Da du nguoi trong phong\n");
            gotoxy(20,3);
            printf("Dang co %s nguoi trong phong\n",numberUser);
            gotoxy(20,4);
            printf("Danh sach ten nguoi choi : %s\n",nameUser);
            sleep(2);
            printf("Chuan bi bat dau tro choi\n");
            sleep(1);
            break;
        }
    }

    setSupport();

    playgame();
    
}

void guide(){
    char input;
    system("clear");
    printf("The le tro choi\n");
    printf("Moi nguoi choi co 3 su tro giup\n");
    printf("1.50-50\n");
    printf("2.Bo luot\n");
    printf("3.Goi y\n\n");
    printf("Tro choi ket thuc khi\n");
    printf("1.Het cau hoi\n");
    printf("2.Chi con 1 nguoi hoac khong con ai\n\n");
    printf("Ban se thay xep hang cua minh khi ket thuc tro choi\n");
    printf("Chuc ban choi game vui ve :)\n\n");
    printf("An enter de quay lai");

    while (1)
    {
        input = getch();

        if(input == 10){
            prepare();
        }
    }
    
}

void prepare(){
    int count = 1;
    char input;

    while (1)
    {
        system("clear");

        if(count == 1){
            printf("\033[0;34m");
            printf("ID ROOM : %s\n",maRoom);
            printf("\033[0m");
            printf("\033[0;33m");
            gotoxy(25,2);
            printf("Bat dau\n");
            printf("\033[0m");
            printf("\033[0;32m");
            gotoxy(21,3);
            printf("The le tro choi\n");
            gotoxy(26,4);
            printf("THoat\n");
            printf("\033[0m");
        }else if(count == 2){
            printf("\033[0;34m");
            printf("ID ROOM : %s\n",maRoom);
            printf("\033[0;34m");
            printf("\033[0;32m");
            gotoxy(25,2);
            printf("Bat dau\n");
            printf("\033[0m");
            printf("\033[0;33m");
            gotoxy(21,3);
            printf("The le tro choi\n");
            printf("\033[0m");
            printf("\033[0;32m");
            gotoxy(26,4);
            printf("THoat\n");
            printf("\033[0m");
        }else{
            printf("\033[0;34m");
            printf("ID ROOM : %s\n",maRoom);
            printf("\033[0m");
            printf("\033[0;32m");
            gotoxy(25,2);
            printf("Bat dau\n");
            gotoxy(21,3);
            printf("The le tro choi\n");
            printf("\033[0m");
            printf("\033[0;33m");
            gotoxy(26,4);
            printf("THoat\n");
            printf("\033[0m");
        }
    
        input = getch();
        if(input == 27){
            input = getch();
            if(input == 91){
                input = getch();
                if(input == 65){
                    count--;
                    if(count < 1){
                        count = 3;
                    }
                }else if(input == 66){
                    count++;
                    if(count > 3){
                        count = 1;
                    }
                }
            }
        }else if(input == 10){
            break;
        }
    }
    
    if(count == 1){
        char buffer[MAX_LINE] = {0};
        strcat(buffer,"start:");
        strcat(buffer,maRoom);
        strcat(buffer," ");
        strcat(buffer,name);
        send(sock,buffer,strlen(buffer),0);
        start();
    }else if(count == 2){
        guide();
    }else{
        char buffer[MAX_LINE] = {0};
        strcat(buffer,"outRoom:");
        strcat(buffer,maRoom);
        send(sock,buffer,strlen(buffer),0);

        getRoom();
    }
}

void historyUser(){
    char buffer[MAX_LINE] = {0};
    nread = recv(sock,buffer,MAX_LINE,0);
    buffer[nread] = 0;

    while (1)
    {
        system("clear");
        char input;
        if(strcmp(buffer,"0") == 0){
            printf("Ban chua co tran dau nao\n");
        }else{
            printf("%s\n",buffer);
        }
        printf("An phim 'enter' de quay lai\n");
        input = getch();

        if(input == 10){
            break;
        }
    }

    getRoom();
}

void getRoom(){
    char buffer[MAX_LINE] = {0};
    int count = 1;

    while (1){
        char input;
        system("clear");
        gotoxy(0,0);
        printf("\033[0;34m");
        printf("Username : %s\n",name);
        gotoxy(20,2);
        printf("Chon phong\n");
        printf("\033[0m");

        for(int i = 0; i < totalRoom; i++){
            if(i + 1 == count){
                printf("\033[0;33m");
                gotoxy(10 + i*6,4);
                printf("%s",room[i].idRoom);
                printf("\033[0m");
            }else{
                printf("\033[0;32m");
                gotoxy(10 + i*6,4);
                printf("%s",room[i].idRoom);
                printf("\033[0m");
            }
        }
        gotoxy(0,5);
        printf("An phim 'o' de dang xuat tai khoan\n");
        printf("An phim 'h' de xem lich su dau cua ban\n");
        input = getch();

        if(input == 27){
            input = getch();
            if(input == 91){
                input = getch();
                if(input == 68){
                    count--;
                    if(count < 1){
                        count = totalRoom;
                    }
                }else if(input == 67){
                    count++;
                    if(count > totalRoom){
                        count = 1;
                    }
                }
            }
        }else if(input == 10){
            char *c;
            char checkRoom[MAX_LINE] = {0};
            char idRoom[10] = {0};
            strcpy(maRoom,room[count-1].idRoom);
            sprintf(idRoom,"%d",count);
            strcat(checkRoom,"idRoom:");
            strcat(checkRoom,idRoom);
            strcat(checkRoom," ");
            strcat(checkRoom,name);
            send(sock,checkRoom,strlen(checkRoom),0);
            nread = recv(sock,buffer,100,0);
            buffer[nread] = 0;

            c = strtok(buffer," ");
            c = strtok(NULL," ");

            if(strcmp(c,"1") == 0){
                prepare();
            }else{
                printf("Phong da day\n");
                sleep(2);
            }
        }else if(input == 111){
            char logout[MAX_LINE] = {0};
            strcat(logout,"logout:");
            strcat(logout,name);
            send(sock,logout,strlen(logout),0);
            nread = recv(sock,buffer,100,0);
            buffer[nread] = 0;
            if(strcmp(buffer,"logout:1") == 0){
                menu();
            }else{
                printf("Dang xuat that bai\n");
                sleep(2);
            }
        }else if(input == 104){
            char history[MAX_LINE] = {0};
            strcat(history,"history:");
            strcat(history,name);
            send(sock,history,strlen(history),0);

            historyUser();
        }
    }
}

void login(){
    while (1)
    {
        char buffer[MAX_LINE] = {0};
        char account[20] = {0};
        char password[10] = {0};
        printf("\033[0;34m");
        printf("=======Dang nhap======\n\n");
        printf("\033[0m");
        printf("Account : "); scanf("%s",account);
        printf("Password : "); scanf("%s",password); getchar();
        strcat(buffer,"login:");
        strcat(buffer,account);
        strcat(buffer," ");
        strcat(buffer,password);
        send(sock,buffer,strlen(buffer),0);
        nread = recv(sock,buffer,100,0);
        buffer[nread] = 0;
        if(strcmp(buffer,"login:1") == 0){
            char *c;
            printf("\033[0;32m");
            printf("Dang nhap thanh cong\n");
            printf("\033[0m");
            send(sock,"username",100,0);
            nread = recv(sock,buffer,100,0);
            buffer[nread] = 0;
            c = strtok(buffer,":");
            c = strtok(NULL," ");
            strcpy(name,c);

            send(sock,"room",100,0);
            nread = recv(sock,buffer,100,0);
            buffer[nread] = 0;
            room = NULL;
            totalRoom = 0;
            room = (Room *) calloc(100,sizeof(Room));

            c = strtok(buffer," ");
            while (c != NULL)
            {
                strcpy(room[totalRoom].idRoom,c);
                c = strtok(NULL," ");
                totalRoom++;
            }
            sleep(2);
            break;
        }else{
            printf("\033[0;31m");
            printf("Tai khoan hoac mat khau khong chinh xac\n");
            printf("\033[0m");
            sleep(2);
        }
    }
}

void registe(){
    while (1)
    {
        char buffer[MAX_LINE] = {0};
        char account[20] = {0};
        char password[10] = {0};
        char repassword[10] = {0};
        printf("\033[0;34m");
        printf("=======Dang ky======\n\n");
        printf("\033[0m");
        printf("Account : "); scanf("%s",account);
        printf("Password : "); scanf("%s",password); 

        while (1)
        {
            printf("Re-Password : "); scanf("%s",repassword); 
            if(strcmp(password,repassword) == 0){
                break;
            }else{
                sleep(1);
                printf("\033[0;31m");
                printf("Wrong Re-Password\n");
                printf("\033[0m");
            }
        }
        
        strcat(buffer,"register:");
        strcat(buffer,account);
        strcat(buffer," ");
        strcat(buffer,password);
        send(sock,buffer,strlen(buffer),0);
        nread = recv(sock,buffer,100,0);
        buffer[nread] = 0;
        if(strcmp(buffer,"register:1") == 0){
            printf("\033[0;32m");
            printf("Dang ky thanh cong\n");
            printf("\033[0m");
            sleep(2);

            printf("\n");
            while (1)
            {
                char mess[MAX_LINE] = {0};
                char username[20] = {0};
                char enter;
                printf("Enter username : "); scanf("%s",username);
                strcpy(mess,"registerUsername:");
                strcat(mess,username);
                send(sock,mess,strlen(mess),0);
                nread = recv(sock,buffer,100,0);
                buffer[nread] = 0;

                if(strcmp(buffer,"registerUsername:1") == 0){
                    printf("\033[0;32m");
                    printf("Tao username thanh cong\n");
                    printf("\033[0m");
                    sleep(2);
                    printf("An enter de quay lai menu\n");
                    while (1)
                    {
                        enter = getch();
                        if(enter == 10){
                            menu();
                        }
                    }
                }else{
                    printf("\033[0;31m");
                    printf("Username da ton tai\n");
                    printf("\033[0m");
                    sleep(2);
                }
            }
        }else{
            printf("\033[0;31m");
            printf("Tai khoan da ton tai\n");
            printf("\033[0m");
            sleep(2);
        }
    }
    
}

void menu(){
    int count = 1;
    char input;

    while (1)
    {
        system("clear");
        printf("\033[0;34m");
        gotoxy(20,0);
        printf("Chao mung ban den voi Ai la trieu phu\n");
        printf("\033[0m");
        if(count == 1){
            printf("\033[0;33m");
            gotoxy(34,3);
            printf("Dang nhap\n");
            printf("\033[0m");
            printf("\033[0;32m");
            gotoxy(35,4);
            printf("Dang ky\n");
            printf("\033[0m");
            printf("\033[0;32m");
            gotoxy(36,5);
            printf("Thoat\n");
            printf("\033[0m");
        }else if(count == 2){
            printf("\033[0;32m");
            gotoxy(34,3);
            printf("Dang nhap\n");
            printf("\033[0m");
            printf("\033[0;33m");
            gotoxy(35,4);
            printf("Dang ky\n");
            printf("\033[0m");
            printf("\033[0;32m");
            gotoxy(36,5);
            printf("Thoat\n");
            printf("\033[0m");
        }else{
            printf("\033[0;32m");
            gotoxy(34,3);
            printf("Dang nhap\n");
            printf("\033[0m");
            printf("\033[0;32m");
            gotoxy(35,4);
            printf("Dang ky\n");
            printf("\033[0m");
            printf("\033[0;33m");
            gotoxy(36,5);
            printf("Thoat\n");
            printf("\033[0m");
        }

        input = getch();
        if(input == 27){
            input = getch();
            if(input == 91){
                input = getch();
                if(input == 65){
                    count--;
                    if(count < 1){
                        count = 3;
                    }
                }else if(input == 66){
                    count++;
                    if(count > 3){
                        count = 1;
                    }
                }
            }
        }else if(input == 10){
            break;
        }
    }

    system("clear");
    if(count == 1){
        login();
        getRoom();
    }else if(count == 2){
        registe();
    }else{
        exit(0);
    }
}

int main(int argc, char const *argv[]) { 
    struct sockaddr_in sAddr;    
    memset((void *) &sAddr, 0, sizeof(struct sockaddr_in));
	sAddr.sin_family = AF_INET;
	// sAddr.sin_addr.s_addr = INADDR_ANY;
	// sAddr.sin_port = 0;

	sock = socket(AF_INET, SOCK_STREAM, 0);
	bind(sock, (const struct sockaddr *) &sAddr, sizeof(sAddr));

	sAddr.sin_addr.s_addr = inet_addr("3.132.159.158");
	sAddr.sin_port = htons(17486);

	if (connect(sock, (const struct sockaddr *) &sAddr, sizeof(sAddr)) != 0) 
	{
		perror("client");
		return 0;
	}

    while (1)
    {
       menu();
    }
    

    close(sock);
    return 0; 
} 