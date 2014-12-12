
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#include <QApplication>
#include <QMainWindow>
#include <qwidget.h>

#include <QObject>
#include <QTimer>
#include <QLabel>
#include <QImage>
#include <QPixmap>
#include "imageviewer.h"
#include "./PvApi.h"


/* headers para socket 
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

#define PORTA 2000
#define LEN 4096
struct sockaddr_in local;
struct sockaddr_in remoto;

*/

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
    Worker worker ;


// Socket_programming //
/*

int sockfd;  //descrito socket 

int client; 
int len = sizeof(remoto);
int slen;
char buffer[4096];

//int socket(int domain, int type, int protocol);


sockfd = socket(AF_INET, SOCK_STREAM, 0);


if (sockfd == -1) {

qDebug << "Error in creating socket";
return false;
//exit(1);

} 
else 
qDebug << "socket created succesfully";





 
local.sin_family  			=  AF_INET;
local.sin_port				=  htons(PORTA);
//local.sin_addr.saddr		= inet_addr("192.168.254.1");

memset(local.sin_zero, 0X0, 8);   

//int bind(int sockfd, const struct sockaddr *addr,socklen_t addrlen);

if (bind(sockfd, (struct sockaddr *)&local,sizeof(local)) == -1){ 

qDebug << "Error in bind";;

//exit(1);
return false;
}

//int listen(int sockfd, int backlog);

listen(sockfd,1);

//int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);



if ( (client = accept(socketfd, (struct sockaddr*)&remoto, &len)) == -1 ) { 

qDebug << "accept error ";

return false;

}

strcpy(buffer,"Welcome!\n\0");

if( send(client,buffer, strlen(buffer),0)) { 

qDebug << "Data send to client\n";

while(1){

if(slen = recv(client,buffer,LEN,0)>0){

buffer[slen-1] = '\0';

qDebug <<"data received by client %s \n",buffer;

close (client);
break;

}


}


}


close(sockfd);
qDebug << "server error";





*/

// Socket_programming //



	return app.exec();

}
















