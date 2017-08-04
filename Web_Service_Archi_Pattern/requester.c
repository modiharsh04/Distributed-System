/*
    C socket SERVICE_REQUESTER, connects to broker and also provider but as a saperate connection and one at a time. it is sequential.
    GROUP MEMBERS = HARSH MODI,1457621
                    PURVISH OZA,1500498

    implementation : FILE_NAME [BROKER||PROVIDER_IP] [PORT]
*/

#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <fcntl.h> // for open
#include <unistd.h> // for close


    void brake(char *);

    int  portno;
    struct hostent *server;
    char *server_name;

    char message[256];
    char *services[25] = {"AddTwoNumbers","SubtractTwoNumbers","MultiplyTwoNumbers","DivideTwoNumbers",
    "Square","SquareRoot","Area","Volume","Perimeter","Circumference","SurfaceArea","Integrate",
    "Differentiate","Power","Logarithm","StringLength","Encryption","Decryption","EdgeDetection",
    "FFT","RayTracing","VolumeRendering","ZBuffer","TextureMapping","MotionBlur"};

    int main(int argc , char *argv[])
    {
        int sock;
        struct sockaddr_in serv_addr;

        int i,j;

        if(argc<3){
            fprintf(stderr, "usage %s hostname port\n",argv[0] );
            exit(0);
        }

        portno = atoi(argv[2]);

    //Create socket
        sock = socket(AF_INET , SOCK_STREAM , 0);
        if (sock == -1)
        {
            printf("Could not create socket");
        }

        server = gethostbyname(argv[1]);
        server_name = argv[1];
        if(server == NULL){
            fprintf(stderr, "ERROR no such host\n" );
            exit(0);
        }

        bzero((char *) &serv_addr,sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        bcopy((char *) server -> h_addr,(char *)&serv_addr.sin_addr.s_addr, server -> h_length);
        serv_addr.sin_port = htons( portno );

    //Connect to remote server
        if (connect(sock , (struct sockaddr *)&serv_addr , sizeof(serv_addr)) < 0)
        {
            perror("connect failed. Error");
            return 1;
        }

        if (portno == 5001)
        {

            printf("Available services:\n");
            for(i=0,j=1;i<25;i++,j++){
                printf("%d. %s\n", j,services[i]);
            }

            bzero(message,256);
            sprintf(message,"%d\n",1);

        //Send some data
            if( send(sock , message , strlen(message) , 0) < 0)
            {
                puts("Send failed");
                return 1;
            }

            printf("Enter Number before service from the list :\n");
            bzero(message,256);
            fgets(message,256, stdin);
        /* Send message to the server */
            if( send(sock , message , strlen(message) , 0) < 0)
            {
                puts("Send failed");
                return 1;
            }

            bzero(message,256);
            if( recv(sock , message , 256 , 0) < 0)
            {
                puts("recv failed");
                return 0;
            }
            char *msg = message;
            brake(msg);

        }else{

            bzero(message,256);
            if( recv(sock , message , 256 , 0) < 0)
            {
                puts("recv failed");
                return 0;
            }
            char *msg = message;
            printf("%s\n", msg);
            brake(msg);
            bzero(msg,256);
            printf("Enter service number:");
            scanf("%s",msg);
            if( send(sock , msg , strlen(message) , 0) < 0)
            {
                puts("Send failed");
                return 1;
            }
            bzero(message,256);
            if( recv(sock , message , 256 , 0) < 0)
            {
                puts("recv failed");
                return 0;
            }
            printf("%s\n", message);            
        }

        close(sock);
        return 0;
    }

void brake(char *str){  //because cannot use break ;)

    char *token = strtok(str,",");
    char *s[20];
    int i = 0 , n;
    while (token != NULL){
      s[i] = token;
      token = strtok(NULL,",");
      ++i;
  }
  if(portno == 5001){
    printf("Asked service are provided by %s server(s):\n\n",s[0]);
    printf("Server |    IP   | PORT\n");
    printf("---------------------------\n");
    i=1;
    while(i <= atoi(s[0])){
      printf("%d  -->  %s\n",i,s[i]);
      ++i;
  }
}
else{
    printf("Service(s) provided by %s:%d\n\n",server_name,portno);
    i=1;
    int f;
    while(i <= atoi(s[0])){
        n = atoi(s[i]);
        printf("%d  -->  %s\n",++n,services[n]);
        ++i;
    }
}
}