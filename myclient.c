#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

// 文本行长度
#define SEGLEN 54
#define INLEN 31
#define FLAGS 2
//SEGLEN + FLAGS + DATALEN
#define SENDLEN 87 
#define RECVLEN 183

char inputline[INLEN];
char sendline[SENDLEN];
char recvline[RECVLEN];

char city_name[INLEN];

//time
int year;
int month;
int day;

//weather
char* weather(int w){
    if(w == '\x00'){
        return "overcast";
    }
    else if(w == '\x01'){
        return "sunny";
    }
    else if(w == '\x02'){
        return "cloudy";
    }
    else if(w == '\x03'){
        return "rain";
    }
    else if(w == '\x04'){
        return "fog";
    }
    else if(w == '\x05'){
        return "rainstorm";
    }
    else if(w == '\x06'){
        return "thunderstorm";
    }
    else if(w == '\x07'){
        return "breeze";
    }
    else if(w == '\x08'){
        return "Please guess: ZmxhZ3tzYW5kX3N0MHJtfQ==";
    }
    else{
        perror("Problem in weather");
        exit(1);
    }
}
//send flags
char _chosen_city = '\x01'; //01:未选择城市 02:已选择了城市
char _data_num = '\x00';   //00:初始 01:查一天 02:查三天

void parse_recv_flags(char *recvline) {
    if(recvline[0] == '\x01') {
        _chosen_city = '\x02';
        strcpy(city_name, inputline);
        printf("Please enter the given number to query\n1.today\n2.three days from today\n3.custom day by yourself\n(r)back,(c)cls,(#)exit\n===================================================\n");
        }
    else if(recvline[0] == '\x02') {
        _chosen_city = '\x01';
        printf("Sorry, Server does not have weather information for city %s!\n", inputline);
        printf("Welcome to NJUCS Weather Forecast Demo Program!\nPlease input City Name in Chinese pinyin(e.g. nanjing or beijing)\n(c)cls,(#)exit\n");
        }
    else if(recvline[0] == '\x03') {
        year = recvline[32]*16*16+recvline[33];
        month = recvline[34];
        day = recvline[35];
        if(recvline[1]=='\x41'){
                if(recvline[36] == 1){
                    printf("City: %s  Today is: %d/%d/%d  Weather information is as follows:\nToday's Weather is: %s;  Temp:%02d\n",
                     city_name, year, month, day, weather(recvline[37]), recvline[38]);
                }
                else{
                    printf("City: %s  Today is: %d/%d/%d  Weather information is as follows:\nThe %dth day's Weather is: %s;  Temp:%02d\n",
                     city_name, year, month, day, recvline[36], weather(recvline[37]), recvline[38]);  
                }
            }
        else if(recvline[1]=='\x42'){
                    printf("City: %s  Today is: %d/%d/%d  Weather information is as follows:\nThe 1th day's Weather is: %s;  Temp:%02d\nThe 2th day's Weather is: %s;  Temp:%02d\nThe 3th day's Weather is: %s;  Temp:%02d\n",
                     city_name, year, month, day, weather(recvline[37]), recvline[38],
                                                weather(recvline[39]), recvline[40],
                                                weather(recvline[41]), recvline[42]);  
        }
        else{
            printf("%x", recvline[1]);
            perror("Problem in data number");
            exit(1);
        }
    }
}

int main(int argc, char **argv){
    // 检查程序参数是否符合要求
    if (argc != 1){
        perror("Just open the program, no need for parameters");
        exit(1);
    }

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd == -1){
        perror("Problem in creating the socket");
        exit(1);
    }

    // 填充服务器 套接字地址
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("47.105.85.28");
    servaddr.sin_port = htons(4321);

    // 与服务器建立连接
    int connect_status = connect(sockfd, (struct sockaddr*) &servaddr, sizeof(servaddr));
    if(connect_status == -1){
        perror("Problem in connecting to the server");
        exit(1);
    }

    memset(&sendline, 0, SENDLEN);
    memset(&recvline, 0, RECVLEN);
    
    printf("Welcome to NJUCS Weather Forecast Demo Program!\nPlease input City Name in Chinese pinyin(e.g. nanjing or beijing)\n(c)cls,(#)exit\n");
    
    while (fgets(inputline, INLEN, stdin) != NULL)
    {   
        inputline[strlen(inputline) - 1] = '\x00';
        if(_chosen_city == '\x01'){
            if(strcmp(inputline, "c") == 0){
                system("clear");
                printf("Welcome to NJUCS Weather Forecast Demo Program!\nPlease input City Name in Chinese pinyin(e.g. nanjing or beijing)\n(c)cls,(#)exit\n");
                continue;
            }
            else if((strcmp(inputline, "#") == 0)){
                exit(0);
            }
            //合并前缀flags
            sprintf(sendline, "%c%c%s", _chosen_city, _data_num, inputline);
        }

        else if(_chosen_city == '\x02'){
            if(strcmp(inputline, "r")==0){
                _chosen_city = '\x01';
                system("clear");
                printf("Welcome to NJUCS Weather Forecast Demo Program!\nPlease input City Name in Chinese pinyin(e.g. nanjing or beijing)\n(c)cls,(#)exit\n");
                continue;
            }
            else if(strcmp(inputline, "c")==0){
                system("clear");
                printf("Please enter the given number to query\n1.today\n2.three days from today\n3.custom day by yourself\n(r)back,(c)cls,(#)exit\n===================================================\n");
                continue;
            }
            else if(strcmp(inputline, "#")==0){
                exit(0);
            }
            //合并前缀flags
            if(strcmp(inputline, "1")==0){
                _data_num = '\x01';
                sprintf(sendline, "%c%c%s", _chosen_city, _data_num, city_name);
                sendline[32] = '\x01';
            }
            else if(strcmp(inputline, "2")==0){
                _data_num = '\x02';
                sprintf(sendline, "%c%c%s", _chosen_city, _data_num, city_name);
                sendline[32] = '\x03';
            }
            else if(strcmp(inputline, "3")==0){
                day_input:
                printf("Please enter the day number(below 10, e.g. 1 means today):");
                fgets(inputline, INLEN, stdin);
                if(0<atoi(inputline) && atoi(inputline)<10){
                    _data_num = '\x01';
                    sprintf(sendline, "%c%c%s", _chosen_city, _data_num, city_name);
                    sendline[32] = atoi(inputline);
                }
                else{
                    printf("Input error\n");
                    goto day_input;
                }
            }
            else{ 
                perror("valid input");
                exit(1);
            }
        }
        else{
            perror("Problem in flags");
            exit(1);
        }
        //发送到服务器
        int send_status = send(sockfd, sendline, SENDLEN, 0);
        if(send_status == -1){
            perror("Problem in sending to the server");
            exit(1);
        }
        
        //接受服务器的响应
        int recv_status = recv(sockfd, recvline, RECVLEN, 0);
        if (recv_status == 0){
            perror("The server terminated prematurely");
            exit(1);
        }
        else if(recv_status == -1){
            perror("Problem in receiving the server's response");
            exit(1);
        }
        //解析flags
        parse_recv_flags(recvline);
        memset(inputline, 0, RECVLEN);
        memset(sendline, 0, SENDLEN);
        memset(recvline, 0, RECVLEN);
    }
    close(sockfd);
    exit(0);
}
