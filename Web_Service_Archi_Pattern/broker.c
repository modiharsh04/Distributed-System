/*
    C socket BROKER, handles multiple clients & servers using threads.
    GROUP MEMBERS = HARSH MODI,1457621
                    PURVISH OZA,1500498
    listenng on port_no = 5001
*/

#include <stdio.h>
#include <string.h> 
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>

    int read_size;
    char *message , client_message[256];

    void update_ip(int arr[]);
    void *client_work(void *);
    void *server_work(void *);
    void brake(char *,int *);
    void service_search(int, void *);
    void update_DB(char *, int);

    int server_counter;
    int server[12][5] = {};
    int provider[25][25] = {};
    int memory_in_use;
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

    int main(int argc , char *argv[])
    {
      int socket_desc,thread_count=0 , client_sock , c , *new_sock;
      memory_in_use = 0;
      server_counter = 0;
      struct sockaddr_in server , client;
      pthread_t sniffer_thread;
      int ip_arr[4];

    //Create socket
      socket_desc = socket(AF_INET , SOCK_STREAM , 0);
      if (socket_desc == -1)
      {
        printf("Could not create socket");
      }
      printf("Socket created\n");

    //Prepare the sockaddr_in structure
      server.sin_family = AF_INET;
      server.sin_addr.s_addr = INADDR_ANY;
      server.sin_port = htons( 5001 );

    //Bind
      if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
      {
        //print the error message
        perror("bind failed. Error");
        return 1;
      }
      printf("bind done\n");

    //Listen
      listen(socket_desc , 3);

    //Accept incoming connections.
      printf("Waiting for incoming connections...\n");
      c = sizeof(struct sockaddr_in);

      while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
      {
        printf("Connection accepted\n");

        //1. getting the flag that says if it is requester or provider
        bzero(client_message,256);
        read_size = recv(client_sock , client_message , 256 , 0);
        if(read_size == 0)
        {
          printf("Client disconnected\n");
          fflush(stdout);
        }
        else if(read_size == -1)
        {
          perror("recv failed");
        }

        //if flag is 1 -> it is a requester
        if(strcmp(client_message,"1\n") == 0){
          new_sock = malloc(1);     //provide space for new_sock dynamically
          *new_sock = client_sock;

          //creating new thread for requester
          if( pthread_create( &sniffer_thread , NULL ,  client_work , (void*) new_sock) < 0)
          {
            perror("could not create thread");
            return 1;
          }
        }
        //if flag is 2 -> it is a provider
        else if(strcmp(client_message,"2\n") == 0){

          //taking ip address from socket of provider
          ip_arr[0] = (int) (client.sin_addr.s_addr&0xFF);
          ip_arr[1] = (int)((client.sin_addr.s_addr&0xFF00)>>8);
          ip_arr[2] = (int)((client.sin_addr.s_addr&0xFF0000)>>16);
          ip_arr[3] = (int)((client.sin_addr.s_addr&0xFF000000)>>24);
          update_ip(ip_arr);    //update ip of provider at the global variable

          new_sock = malloc(1);
          *new_sock = client_sock;
          //creating thread of server.
          if( pthread_create( &sniffer_thread , NULL ,  server_work , (void*) new_sock) < 0)
          {
            perror("could not create thread");
            return 1;
          }
        }
      }

      if (client_sock < 0)
      {
        perror("accept failed");
        return 1;
      }

      return 0;
    }


//thread handler function, which handles the requester.
 void *client_work(void *socket_desc){
 	int sock = *(int *) socket_desc;

 	bzero(client_message,256);
 	read_size = recv(sock , client_message , 256 , 0);
 	if(read_size == -1)
 	{
 		perror("recv failed");
 	}
 	printf("%s\n", client_message);

  pthread_mutex_lock(&mutex); //mutex_lock & condition variable use.
  while(memory_in_use == 1)
  {
    pthread_cond_wait(&cond, &mutex);
  }
  memory_in_use =1;
  service_search(atoi(client_message),(void *)socket_desc);
  memory_in_use =0;
  pthread_cond_signal(&cond);
  pthread_mutex_unlock(&mutex);

  //Free the socket pointer
  free(socket_desc);
  printf("Client disconnected\n");
  return NULL;
}

void update_ip(int arr[]){
  int i;
  for(i=0;i<5;i++){
    server[server_counter][i] = arr[i];
  }
}

//thread handler function for handling provider.
void *server_work(void *socket_desc){
  int sock = *(int *) socket_desc;
  int flag=0,z=0,i;

  bzero(client_message,256);
  while(read_size = recv(sock , client_message , 256 , 0)>0){
    message = client_message;
    if(flag == 0){    // flag for notifying already inserted into database, so next time only update.
      pthread_mutex_lock(&mutex);
      while(memory_in_use == 1)
      {
        pthread_cond_wait(&cond, &mutex);
      }
      memory_in_use =1;
      brake(message,&z);  //breaking of client_message to update database.
      memory_in_use =0;
      flag = 1;
      pthread_cond_signal(&cond);
      pthread_mutex_unlock(&mutex);

    }else if(flag == 1){
        pthread_mutex_lock(&mutex);
        while(memory_in_use == 1)
        {
          pthread_cond_wait(&cond, &mutex);
        }
        memory_in_use = 1;
      update_DB(message,z); //Updating the database on the data updated by the server.
      memory_in_use =0;
      pthread_cond_signal(&cond);
      pthread_mutex_unlock(&mutex);
    }
    message = "I got your message server\n";
    write(sock , message , strlen(message));
    bzero(client_message,256);
  }
  if(read_size == -1)
  {
    perror("recv failed");
  }
  free(socket_desc);
  printf("Server %d disconnected\n",z);
  return 0;
}
//function for updating database of servers and their services every 120s after once insterted.
void update_DB(char *str,int z){
  char *token = strtok(str,",");
  char *s[20];
  int i = 0;
  while (token != NULL){
    s[i] = token;
    token = strtok(NULL,",");
    ++i;
  }
  provider[z][0] = atoi(s[1]);
  i=1;
  int j=2;
  while(i<=provider[z][0]){
    provider[z][i] = atoi(s[j]);
    printf("%d\n", provider[z][i]);
    ++i;
    ++j;
  }
}
//for inserting server details into database for the first time.
void brake(char *str,int *z){  //because cannot use break ;)

  char *token = strtok(str,",");
  char *s[20];
  int i = 0;
  while (token != NULL){
    s[i] = token;
    token = strtok(NULL,",");
    ++i;
  }
  server[server_counter][4] = atoi(s[0]);
  provider[server_counter][0] = atoi(s[1]);
  i=1;
  int j=2;
  while(i<=provider[server_counter][0]){
    provider[server_counter][i] = atoi(s[j]);
    printf("%d\n", provider[server_counter][i]);
    ++i;
    ++j;
  }
  *z = server_counter; //server_conter, keeps track of handled servers
  ++server_counter;
}
//Search algorithm on server - services database for requester's request of perticular service.
void service_search(int s, void *socket_desc){
  int sock = *(int*) socket_desc;
  int i=0,j,k=0;
  int a[10];
  printf("Searchiing for service for %d servers\n",server_counter);
  while(i<server_counter){
    j=1;        
    while(j<=provider[i][0]){
      if (provider[i][j] == s)
      {
        a[k] = i;
        ++k;
        break;
      }
      ++j;
    }
    ++i;
  }
  i=0;
  int z=0;
  bzero(client_message,256);
  sprintf(client_message,"%d",k);
  while(i<k){
    z = a[i];
    strcpy(client_message + strlen(client_message),",");
    sprintf(client_message + strlen(client_message),"%d.%d.%d.%d:%d",server[z][0],server[z][1],server[z][2],server[z][3],server[z][4]);
    ++i;
  }
  write(sock , client_message , strlen(client_message));
}