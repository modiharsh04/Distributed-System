/*
    C socket SERVICE_PROVIDER, handles multiple clients using fork(multiprocess)
                                      & BROKER using thread (Updates on 120 seconds).
    GROUP MEMBERS = HARSH MODI,1457621
                    PURVISH OZA,1500498

    implementation : FILE_NAME [BROKER_IP] [BROKER_PORT] [PORT_NO FOR THIS SERVER]
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>

// All functions used in program.
void doprocessing (int);
void *client_start();
void clientProcess();
void serverPrcess();
int services_provide();

//global variables.
int sockfd,newsockfd,pid, portno, clilen,a,b,c,d,n,i;
int port_being_client, sockfd1;
char buffer[256];
struct sockaddr_in serv_addr, cli_addr;
struct hostent *server;
char *port;

int *serves;
sem_t mutex;

int main( int argc, char *argv[] ) {

  pthread_t server_thread;
  int shmid, count = 16;
  key_t key = 123; //key for shared memory

  //Initialization of SEMAPHORE implementation.
  if( sem_init(&mutex,1,1) < 0)
    {
      perror("semaphore initilization");
      exit(0);
    }

  shmid = shmget(key, count*sizeof(int), 0666|IPC_CREAT);
  if (shmid < 0){
    perror("shmget");
    exit(0);
  }
//Initialization of array pointer to the shared memory.
serves = (int *)shmat(shmid, 0, 0);

  //Checking of valid inputs.
  if(argc<3){
    fprintf(stderr, "usage %s hostname_broker portno_broker portno_your\n",argv[0] );
    exit(0);
  }
  port = argv[3];
  server = gethostbyname(argv[1]);
  if(server == NULL){
    fprintf(stderr,"ERROR no such host\n");
    exit(0);
  }

  port_being_client = atoi(argv[2]);
  portno=atoi(argv[3]);//port no passed as argument

  if( pthread_create( &server_thread , NULL ,  client_start , NULL) < 0)
  {
    perror("could not create thread");
    return 1;
  }

  //Process that will handle the server's functionality.
  serverPrcess();

  //Deattachment of shared memory.
  shmdt((void *) serves);
  return 0;
}

//Child process functionality declaration, which will handle every requester personally.
void doprocessing (int newsockfd) {
  int n, count = 16;
  int shmid, *array;
  char msg[256];
  key_t key = 123;

  shmid = shmget(key, count*sizeof(int), IPC_EXCL);
  if (shmid < 0){
    perror("shmget");
    exit(0);
  }
  array = shmat(shmid, 0, SHM_RDONLY);
  
  a = (int) (cli_addr.sin_addr.s_addr&0xFF);
  b = (int)((cli_addr.sin_addr.s_addr&0xFF00)>>8);
  c = (int)((cli_addr.sin_addr.s_addr&0xFF0000)>>16);
  d = (int)((cli_addr.sin_addr.s_addr&0xFF000000)>>24);

  printf("\nclient %d.%d.%d.%d : %d connected....\n", a,b,c,d,cli_addr.sin_port);

  bzero(msg,256);
  i = 0;
  sem_wait(&mutex);   //waiting for semaphore signal to access the shared memory.
  while(i<=array[0]){
    sprintf(msg + strlen(msg),"%d,",array[i]);
    ++i;
  }
  sem_post(&mutex);   //Signalling waiting processes for shared memory access.
  n = write(newsockfd, msg, strlen(msg));
  if (n < 0){
    perror("ERROR reading from socket !!");
    exit(1);
  }

  bzero(msg,256);
  n = read(newsockfd,msg,256);
  if (n<0)
  {
    perror("ERROR reading from socket !!");
    exit(1);
  }
  printf("Client entered %s\n",msg);
  bzero(msg,256);
  strcpy(msg,"you have been provided with the service\n");
  n = write(newsockfd, msg, strlen(msg));
  if (n < 0){
    perror("ERROR reading from socket !!");
    exit(1);
  }
  printf("\nclient %d.%d.%d.%d : %d disconnected....\n", a,b,c,d,cli_addr.sin_port);
  close(newsockfd);
  shmdt((void *) array);
}

//Client functionality starts here. ie from provider ro broker
void *client_start(){
    /* First call to socket() function */
  sockfd = socket(AF_INET, SOCK_STREAM, 0);

  if (sockfd < 0) {
    perror("ERROR opening socket");
    exit(1);
  }

  bzero((char *) &serv_addr, sizeof(serv_addr));

  serv_addr.sin_family = AF_INET;
  bcopy((char *) server -> h_addr,(char *)&serv_addr.sin_addr.s_addr, server -> h_length);
  serv_addr.sin_port = htons(port_being_client);


  if (connect(sockfd, (struct sockaddr *)&serv_addr,sizeof(serv_addr))<0)
  {
    perror("ERROR connecting to server !!");
    exit(1);
  }
  printf("Connected....\n");

    //1. - send to notify this is provider
  bzero(buffer,256);
  sprintf(buffer,"%d\n",2);
  n = write(sockfd,buffer,256);
  if (n<0)
  {
    perror("ERROR writing to socket !!");
    exit(1);
  }
  while(1){
   clientProcess();
   sleep(120);
  }
  close(sockfd);
  return 0;
}

//This is the actual functionality that will be run.
void clientProcess(){
  sem_wait(&mutex);
  serves[0] = services_provide();
  bzero(buffer,256);
  printf("serves[0] : %d\n", serves[0]);
  strcpy(buffer,port);
  strcpy(buffer + strlen(buffer),",");
  i = 0;
  while(i<=serves[0]){
    sprintf(buffer + strlen(buffer),"%d,",serves[i]);
    ++i;
  }
  sem_post(&mutex);

  n = write(sockfd,buffer,strlen(buffer));
  if (n<0)
  {
    printf("2.\n");
    perror("ERROR writing to socket !!");
    exit(1);
  }
    //4.
  bzero(buffer,256);
  n = read(sockfd,buffer,256);
  if (n<0)
  {
    printf("3.\n");
    perror("ERROR reading from socket !!");
    exit(1);
  }
  printf("%s", buffer);
}

//Server's socket creation and forking for handling multiple requester using multi-processing.
void serverPrcess(){
  sockfd1 = socket(AF_INET, SOCK_STREAM, 0);
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(portno);

   /* Now bind the host address using bind() call.*/
  if (bind(sockfd1, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
    perror("ERROR on binding");
    exit(1);
  }

   /* Now start listening for the clients, here process will
      * go in sleep mode and will wait for the incoming connection
   */

      listen(sockfd1,5);
      clilen = sizeof(cli_addr);

      for(;;)
      {
      /* Accept actual connection from the client */
        newsockfd = accept(sockfd1, (struct sockaddr *)&cli_addr, &clilen);
        if (newsockfd < 0) {
         perror("ERROR on accept");
         exit(1);
       }
       pid = fork();
       if(pid<0){
        perror("ERROR on fork");
        exit(1);
      }
      if (pid == 0)
      {
        close(sockfd1);
        doprocessing(newsockfd);
        exit(0);
      }
      else{
        close(newsockfd); /*close connection if child cannot be created*/
      }
    }
  }

//Randomize function that will tell the broker that which services this server will provide.
  int services_provide(){
    time_t t;
    srand((unsigned) time(&t));
    int r = 1 + rand()%14;
    printf("provinding %d services: \n", r);
    for (i = 1; i <= r; i++)
    {
      int x = 30,flag = 0;
      int j=1;
      do{
       x = rand()%25;
       do{
        if (serves[j] == x)
        {
          flag = 1;
          break;
        }
        ++j;
      }while(j<=i);
      if(j>i){
        flag = 0;
      }
    }while(flag == 1);
    serves[i] = x;

    printf("%d\n", serves[i]);
  }
  return r;
}
