/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
     int sockfd, newsockfd, portno,i;
     socklen_t clilen;
     char buffer[256];
	char *data[] = {"harsh\0","modi\0","is\0","name\0"};
	i = 0;
	int a[15] = {1,2,3,4,5,6,7,8,9,10};
	for(i=0;i<4;i++){
		printf("%s\n",data[i]);
	}
     struct sockaddr_in serv_addr, cli_addr;
     int n;
     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        error("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              error("ERROR on binding");
     listen(sockfd,5);
     clilen = sizeof(cli_addr);
     newsockfd = accept(sockfd, 
                 (struct sockaddr *) &cli_addr, 
                 &clilen);
     if (newsockfd < 0) 
          error("ERROR on accept");
     bzero(buffer,256);
printf("IP address is: %d\n",(int) inet_ntoa(cli_addr.sin_addr));
     n = write(newsockfd,&a,256);
     if (n < 0) error("ERROR writing to socket");
//~ for(i=0;i<4;i++){
	//~ bzero(buffer,256);
     //~ n = write(newsockfd,data[i],256);
     //~ if (n < 0) error("ERROR writing to socket");
//~ }
     close(newsockfd);
     close(sockfd);
     return 0; 
}
