#include <stdio.h>
#include <stdlib.h>

#include <netdb.h>
#include <netinet/in.h>

#include <string.h>

int sockfd, newsockfd, portno, clilen;
char buffer[256];
char data[256];
struct sockaddr_in serv_addr, cli_addr;
int  n;
FILE *f;

void download();
void s();

int main( int argc, char *argv[] ) {

   /* First call to socket() function */
   sockfd = socket(AF_INET, SOCK_STREAM, 0);

   if (sockfd < 0) {
      perror("ERROR opening socket");
      exit(1);
  }

   /* Initialize socket structure */
  bzero((char *) &serv_addr, sizeof(serv_addr));
  portno = 9999;

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(portno);

   /* Now bind the host address using bind() call.*/
  if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
      perror("ERROR on binding");
      exit(1);
  }

   /* Now start listening for the clients, here process will
      * go in sleep mode and will wait for the incoming connection
   */

      listen(sockfd,5);
      clilen = sizeof(cli_addr);

      for(;;)
      {
      /* Accept actual connection from the client */
          newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);

          if (newsockfd < 0) {
             perror("ERROR on accept");
             exit(1);
         }

      /* If connection is established then start communicating */
        // download();
        s();
     }
     return 0;
 }
 
 void s(){
	char *fn = "HARSH";
	char *ln = "MODI";
	strcpy(buffer,fn);
	n = write(newsockfd,buffer,strlen(buffer));
	if (n < 0) {
		perror("ERROR writing to socket");
		exit(1);
	}
	printf("%s\n",buffer);
	strcpy(buffer,ln);
	n = write(newsockfd,buffer,strlen(buffer));
	if (n < 0) {
		perror("ERROR writing to socket");
		exit(1);
	}
	printf("%s\n",buffer);
}

 void download(){
    //reading file_name
    bzero(buffer,256);
    n = read( newsockfd,buffer,256);

    if (n < 0) {
     perror("ERROR reading from socket");
     exit(1);
 }
 char t[30] = "new/";
 strcat(t,buffer);
      //creating file with file name
 f = fopen(t,"w+");

      //reading file_data
 bzero(data,256);
 n = read( newsockfd,data,256);

 if (n < 0) {
     perror("ERROR reading from socket");
     exit(1);
 }

      //writing data to file
 fprintf(f,data);
 strcat(buffer,"has data : ");
 strcat(buffer,data);

 printf("%s\n", buffer);
      /* Write a response to the client */
 n = write(newsockfd,buffer,strlen(buffer));

 if (n < 0) {
     perror("ERROR writing to socket");
     exit(1);
 }
 fclose(f);
}
