    #include <stdio.h>
    #include <stdlib.h>
    #include <netdb.h>
    #include <netinet/in.h>
    #include <string.h>

int sockfd, portno, n;
struct sockaddr_in serv_addr;
struct hostent *server;

char buffer[256];
char data[256];

FILE *f;

void upload();

int main(int argc, char *argv[]) {


 if (argc < 2) {
  fprintf(stderr,"usage %s hostname\n", argv[0]);
  exit(0);
}

portno = atoi(argv[2]);

       /* Create a socket point */
sockfd = socket(AF_INET, SOCK_STREAM, 0);

if (sockfd < 0) {
  perror("ERROR opening socket");
  exit(1);
}

server = gethostbyname(argv[1]);

if (server == NULL) {
  fprintf(stderr,"ERROR, no such host\n");
  exit(0);
}

bzero((char *) &serv_addr, sizeof(serv_addr));
serv_addr.sin_family = AF_INET;
bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
serv_addr.sin_port = htons(portno);

       /* Now connect to the server */
if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
  perror("ERROR connecting");
  exit(1);
}

       /* Now ask for a message from the user, this message
          * will be read by server
       */

          printf("Do you want to attach a file? : ");
          bzero(buffer,256);
          fgets(buffer,256,stdin);

          if (strcmp(buffer,"yes\n")==0){
            upload();
        }

       /* Now read server response */
       bzero(buffer,256);
       n = read(sockfd, buffer, 256);
       if (n < 0) {
		perror("ERROR reading from socket");
		exit(1);
       }
       upload();
       
      return 0;
  }
  
  void s(){
	  char *fn;
       char *ln;
       bzero(buffer,256);
       n = recv(sockfd, buffer, 256,0);
       if (n < 0) {
		perror("ERROR reading from socket");
		exit(1);
       }
       printf("%d\n",n);
       buffer[n] = '\0'
      printf("%s\n",buffer);
      bzero(buffer,256);
       n = read(sockfd, buffer, 256);
       if (n < 0) {
		perror("ERROR reading from socket");
		exit(1);
       }
       buffer[n] = '\0';
       printf("%s\n",buffer);
	  }

  void upload(){
    char *fcontent = NULL;
    int fsize = 0;

    printf("enter file name : ");
    bzero(buffer,256);
    scanf("%s",&buffer);
    //fgets(buffer,256,stdin);
  f = fopen(buffer,"r");
  if (f == NULL){
    perror("\nin opening file\n");
    exit(1);
  }
  /* Send file_name to the server */
    n = write(sockfd, buffer, strlen(buffer));
    if (n < 0) {
      perror("ERROR writing to socket");
      exit(1);
  }
  bzero(data,256);
  //fgets(data,256,(FILE*)f);
  fseek(f,0,SEEK_END);
  fsize = ftell(f);
  rewind(f);

  fcontent = (char*) malloc(sizeof(char) *fsize);
  fread(fcontent,1,fsize,f);

  //printf("\n%s\n", fcontent);
              //writing data to server
  n = write(sockfd, fcontent, strlen(fcontent));
  if (n < 0) {
      perror("ERROR writing to socket");
      exit(1);
  }
  fclose(f);
}
