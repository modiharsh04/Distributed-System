#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
/*to run this program you need to pass server address and port no 5001 as arguments*/
/*i.e ./client 127.0.0.1 5001*/
int main(int argc, char const *argv[])
{
	int sockfd, portno, n,flag=0,cookies,len;
	struct sockaddr_in serv_addr;
	struct hostent *server;

	char buffer[256];	
	/*checking number of arguments passed */
	if(argc<3){
		fprintf(stderr, "usage %s hostname port\n",argv[0] );
		exit(0);
	}

	portno=atoi(argv[2]);//port no passed as argument


	/*create a socket point*/
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd<0){
		perror("ERROR opening socket !!");
		exit(1);
	}
	/*get server name*/
	server = gethostbyname(argv[1]);
	if(server == NULL){
		fprintf(stderr, "ERROR no such host\n" );
		exit(0);
	}
	/*initialize socket structure*/
	bzero((char *) &serv_addr,sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *) server -> h_addr,(char *)&serv_addr.sin_addr.s_addr, server -> h_length);
	serv_addr.sin_port = htons(5001);

	/* Now connecting to the server */
	if (connect(sockfd, (struct sockaddr *)&serv_addr,sizeof(serv_addr))<0)
	{
		perror("ERROR connecting to server !!");
		exit(1);
	}
	printf("Please enter number\n1 to connect fortune cookie server.\n2 to connect with encryption/decryption server\n3 quit :");
	bzero(buffer,256);
	fgets(buffer,256, stdin);
	/* Send message to the server */
	n = write(sockfd,buffer,strlen(buffer));
	/*client side for fortune server*/
	if(strcmp(buffer,"1\n")==0){
		printf("Enter number of fortune cookies you want : ");
		bzero(buffer,256);
		fgets(buffer,256,stdin);
		cookies = buffer[0]-'0';
		n= write(sockfd,buffer,256);
		if (n<0)
		{
			perror("ERROR writing to socket !!");
			exit(1);
		}
		int i;
		for(i=0;i<cookies;i++){
			/*Now read server response*/
			bzero(buffer,256);
			n = read(sockfd,buffer,256);
			printf("\nfortune cookie %d : %s\n",i+1, buffer);
		}
		printf("Bye Bye from server\n");
	}
	/*Client side for concurrent server*/
	else if(strcmp(buffer,"2\n")==0){
	/*running until quit by client*/
	for(;;){
	/*Now ask for message fromm user and send to server for reading*/
	do{
		printf("Please enter number\n1 for encryption.\n2 for decryption\n3 quit :");
		bzero(buffer,256);
		fgets(buffer,256, stdin);
		if (buffer[0]=='1' || buffer[0]=='2' || buffer[0]=='3')
		{
			flag=1;
		}
		else
			printf("\n **** Invalid Entry ****.\n\n");
	}while(flag==0);	
	/* Send message to the server */
	n = write(sockfd,buffer,strlen(buffer));

	if (n<0)
	{
		perror("ERROR writing to socket !!");
		exit(1);
	}
	if(strcmp(buffer,"1\n")==0){

		printf("\n** ENCRYPTION MODE **\nEnter string :");
		bzero(buffer,256);
		fgets(buffer,256, stdin);

		/* Send message to be encrypted to the server */
		n = write(sockfd,buffer,strlen(buffer));
		if (n<0)
		{
			perror("ERROR writing to socket !!");
			exit(1);
		}

		/*Now read server response*/
		bzero(buffer,256);
		n = read(sockfd,buffer,256);
		if (n<0)
		{
			perror("ERROR writing to socket !!!");
			exit(1);
		}

		printf("\nHere is the encrypted string : %s\n", buffer);
	}
	else if(strcmp(buffer,"2\n")==0){
		printf("\n ** DECRYPTION MODE **\nEnter string :");
		bzero(buffer,256);
		fgets(buffer,256, stdin);
		/* Send message to the server */
		n = write(sockfd,buffer,strlen(buffer));
		if (n<0)
		{
			perror("ERROR writing to socket !!");
			exit(1);
		}
		/*Now read server response*/
		bzero(buffer,256);
		n = read(sockfd,buffer,256);
		if (n<0)
		{
			perror("ERROR writing to socket !!!");
			exit(1);
		}
		printf("\nHere is the decrypted string : %s\n", buffer);
	}
	else if(strcmp(buffer,"3\n")==0){
		printf("\nbye bye client !!\n");
		return 0;
	}
	else{
		printf("Invalid entry : client exits\n");
		break;
	}
	
} /*end of loop*/
}//end of client side oof concurrent server
	return 0;
}