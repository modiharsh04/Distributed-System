#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

void encryption(int sock);
void decryption(int sock);
void doprocessing (int sock) ;
int main(int argc, char const *argv[])
{
	/* Defining variables*/
	int sockfd, newsockfd, portno, client_len, n, pid;
	char buffer[256];
	struct sockaddr_in serv_addr, cli_addr;

	FILE *fp;
	int i=0,m,cookie;
	time_t t;

	/* first call to socket() function */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd<0){
		perror("ERROR OPENING SOCKET");
		exit(1);
	}
	/* initialize socket structure */
	bzero((char *) &serv_addr, sizeof(serv_addr));
	portno=5001;
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);

	/* Binding the host address using bind() call. */
	if(bind(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr))<0){
		perror("ERROR on binding !!");
		exit(1);
	}
	
	/* Listening for client */
	listen(sockfd,5);
	client_len = sizeof(cli_addr);

	/* putting in infinite loop so can accept so many connection*/
	for(;;){
		/* accepting connection from the client at new socket descriptor*/
		newsockfd = accept(sockfd,(struct sockaddr *)&cli_addr,&client_len);
		if (newsockfd<0)
		{
			perror("ERROR on accept !!");
			exit(1);
		}
		printf("\nconnected to client: %d\n",inet_ntoa(cli_addr.sin_addr));
		/*Reading string from client to choose server*/
		bzero(buffer,256);
		n = read(newsockfd,buffer,256);
		if (n<0)
		{
			perror("ERROR reading from socket !!");
			exit(1);
		}
		/*Iterative server - running on parent process*/
		if(strcmp(buffer,"1\n")==0){
			/*reading string from client - number of cookies*/
	   		bzero(buffer,256);
	   		n=read(newsockfd,buffer,256);
	   		if(n<0){
	   			perror("ERROR reading from socket");
	   			exit(1);
	   		}

	   		cookie = buffer[0]-'0'; /*cookies number*/

			printf("\nServer message : cookies send to client\n");
			/*Generating random number to choose fortune cookie from file*/
			srand((unsigned) time(&t));
			m = rand()%50;
			/*opening and extracting data from fortune cookie file*/
			fp = fopen("/home/harsh/Documents/programs/cli-server/fortune.txt","r");
			for(i=0;i<m;i++){
				bzero(buffer,256);
				fgets(buffer,256,(FILE*)fp);
			}
			for (i = m; i < m+cookie;++i)
			{
				bzero(buffer,256);
				fgets(buffer,256,(FILE*)fp);	
				n = write(newsockfd,buffer,256); /*Sending cookies to client*/
				if(n<0){
					perror("ERROR writing to socket !!");
					exit(1);
				}
			}
			fclose(fp);
			close(newsockfd);/*socket closed*/
		}
		/* Creating child process */
		/* This is our concurrent server */
		else if(strcmp(buffer,"2\n")==0){
			pid = fork();
			if(pid<0){
				perror("ERROR on fork");
				exit(1);
			}

			if (pid == 0)
			{
				close(sockfd);
		         doprocessing(newsockfd);
		         exit(0);
			}
			else{
				close(newsockfd); /*close connection if child cannot be created*/
			}
		}
		/*close client connection if it tries invalid entry*/
		else{
			printf("client %d chose to quit !!\n",inet_ntoa(cli_addr.sin_addr.s_addr));
			close(newsockfd);
		}
		
	} /* end of loop */
	return 0;
}
// concurrent server function
void doprocessing (int sock) {
   	int n, mode =0;
   	char buffer[256];
   	/*infinite loop until client quits*/
   	while(1){
	   bzero(buffer,256);
	   n = read(sock,buffer,255);
	   if (n < 0) {
	    perror("ERROR reading from socket");
	    exit(1);
	   }
	   // encryption mode
	   if (strcmp(buffer,"1\n")==0){
	   	encryption(sock);
	   }
	   // decryption mode
	   else if (strcmp(buffer,"2\n")==0){
	   	decryption(sock);
	   }
	   // end of connection on trying invalid entry by client
	   else{
	   	exit(1);
	   }
	   if (n < 0) {
	      perror("ERROR writing to socket");
	      exit(1);
	   }
	} //end of loop
}
/*Encyption function*/
void encryption(int sock){
	int n;
	char buffer[256], encrypted[256];
	/* Read from client */
	bzero(buffer,256);
	n = read(sock, buffer,256);
	if(n<0){
		perror("ERROR reading fromm socket !!");
		exit(1);
	}
	
	printf("Here is the client message : %s\n", buffer);
	printf("Ecrypting message....\n");
	/* Write to client */
	int length = strlen(buffer);
	int i;
	/*Actual encryption takes place here*/
	for (i = 0; i < length; i++)
	{
		 if(((buffer[i]>='a')&&(buffer[i]<'x')) || ((buffer[i]>='A')&&(buffer[i]<'X')))
			 buffer[i]=buffer[i] + (char)3;
		 else if((buffer[i]=='x') || (buffer[i]=='X'))
			 buffer[i]='a';
		 else if((buffer[i]=='y') || (buffer[i]=='Y'))
			 buffer[i]='b';
		 else if((buffer[i]=='z') || (buffer[i]=='z'))
			 buffer[i]='c';
	}
	buffer[i++]='\0';
	n = write(sock,buffer,256);
	if(n<0){
		perror("ERROR writing to socket !!");
		exit(1);
	}
	printf("Ecryption successful..........\n");
}
/*Decryption function*/
void decryption(int sock){
	int n;
	char buffer[256], encrypted[256];
	/* Read from client */
	bzero(buffer,256);
	n = read(sock, buffer,256);
	if(n<0){
		perror("ERROR reading fromm socket !!");
		exit(1);
	}
	printf("Here is the client message : %s\n", buffer);
	printf("Decrypting message....\n");
	/* Write to client */
	int length = strlen(buffer);
	int i;
	/*Actual decryption takes place here*/
	for (i = 0; i < length; i++)
	{
		 if(((buffer[i]>='d')&&(buffer[i]<'z')) || ((buffer[i]>='D')&&(buffer[i]<'Z')))
			 buffer[i]=buffer[i] - (char)3;
		 else if((buffer[i]=='c') || (buffer[i]=='C'))
			 buffer[i]='x';
		 else if((buffer[i]=='b') || (buffer[i]=='B'))
			 buffer[i]='y';
		 else if((buffer[i]=='a') || (buffer[i]=='A'))
			 buffer[i]='z';
	}
	buffer[i++]='\0';
	n = write(sock,buffer,256);
	if(n<0){
		perror("ERROR writing to socket !!");
		exit(1);
	}
	printf("Decryption successful....\n");
}