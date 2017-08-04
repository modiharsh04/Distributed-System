//Harsh Modi - Computer Scientist
//https://www.linkedin.com/in/harsh-modi-36011568



The web Server Architectural Programme;




Broker:
gcc broker.c -o broker -lpthread
./broker
To begin with, in broker file :
C socket BROKER, handles multiple clients & servers using threads.
Listening on port no=5001
First of all we have created a socket then regarding to that socket ..bind,listen
and connection accpetance will be done.
To decide who is requester or provider we have flag, for value 1 it is requester
and for value 2 it is provider.
Then we have created thread of server with two thread handler function, one for
requester and one for provider.
We provided function update _DB(char *str,int z)for updating database of servers
and their services every 120s after once insterted.
We have use brake() function to insert value in database for the first time.
for inserting server details into database for the first time there is a function
void brake(char *str,int *z).
and at the end for requester’s request for particular service it search with
provided function.
PROVIDER:
C socket SERVICE_PROVIDER, handles multiple clients using fork(multiprocess)
& BROKER using thread (Updates on 120 seconds).
implementation :

provider[BROKER_IP] [BROKER_PORT] [5001]

gcc provider.c -o provider -lpthread
./provider
To provide sychronization on shared memeory we have used SEMAPHORE here.
We are also using thread between provider and broker.
Child process I.e doprocessing(int newsockfd) will handle every requester
personally.
After client’s successful implementation , Multiprocessing with a single thread
will run concuurently.

Server's socket creation and forking for handling multiple requester using multiprocessing.

After that connection, server start listening to incoming clients and from this it
will accept clients.
Purpose of Randomize function is that it will tell the broker that which services
will be provided.

REQUESTER:
C socket SERVICE_REQUESTER, connects to broker and also provider but as a
seperate connection and one at a time. It is sequential.
gcc requester.c -o requester
./requester
implementation : Requester [BROKER||PROVIDER_IP][BROKER’s PORT||PROVIDER’s PORT]
To begin with in requester side, we will create socket to establish connection with
remote server on port 5001 and after it will send some message.
After successful connection, requester will ask for particular service with service
number and our broker will send the possible providers with that services to
requester.
After this, requester will connect to provider for that particular service and
provider will send that service to requester.