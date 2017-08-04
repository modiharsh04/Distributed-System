/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include<assert.h>

void error(const char *msg)
{
	perror(msg);
	exit(1);
}

char** str_split(char* a_str, const char a_delim);

int i,n,k;
char *services[2][2] = {{"ADD","SUB"},{"encrypt","Decrypt"}};

void try();



int main(int argc, char *argv[])
{
	//random_work();
	//dou_arr();
	//try();
	//	string_cut();
//char **token = services;



//char buffer[256] = "127.0.0.0";
//char *s[256];
///int a[256];
//int x;
//char *token = strtok(buffer,".");
//i=0;
//while (token != NULL){
//	s[i] = token;
//	printf("%s....",s[i]);
//	a[i] = atoi(s[i]);
//	printf("%d\n",a[i]);	
//	token = strtok(NULL,".");
//	++i;
//}

printf("%s.....%s",services[0][0],services[0][1]);

//buffer = services;
//printf("%s\n",*token);	
//printf("%s\n",*(token+1));

    return 0; 
}

// void dou_arr(){
// 		int arr[5][2] = {{1,1001},{1,2001},{2,3001},{2,2001},{3,4001}};
// 		int a[10];
// 		int r = 0;

// 	random_work();

// 	for(i=0;i<5;i++){
// 		if(arr[i][0] == k){
// 				a[r] = arr[i][1];
// 				++r;
// 			}
// 	}
// 	printf("Service %s is available on %d ports :\n",services[--k],r);
// 	for(i=0;i<r;i++){
// 		printf("%d\n",a[i]);
// 	}
// 	}

// void random_work(){

// 	srand(time(NULL));
// 	int r = rand()%4;
// 	for(i=0,n=1;i<r;i++,n++){
// 		printf("%d. %s\n",n,services[i]);
// 	}
// 	printf("Enter No: ");
// 	scanf("%d",&k);
// 	for(i=0,n=1;i<r;i++,n++){
// 		if(n == k){
// 				printf("You selected %s\n",services[i]);
// 			}
// 	}
// 	}

void try(){
	int a[15];
	int i,j;
	bzero(a,64);
	for (i=0,j=1;i<10;i++,j++){
		a[i] = j;
	}
	i=0;
	while(a[i] != 0){
		printf("%d\n", a[i]);
		++i;
	}
}

void string_cut(){
	char months[] = "JAN,FEB,MAR,APR,MAY,JUN,JUL,AUG,SEP,OCT,NOV,DEC";
    char** tokens;

    printf("months=[%s]\n\n", months);

    tokens = str_split(months, ',');

    if (tokens)
    {
        int i;
        for (i = 0; *(tokens + i); i++)
        {
            printf("month=[%s]\n", *(tokens + i));
            free(*(tokens + i));
        }
        printf("\n");
        free(tokens);
    }
}

char** str_split(char* a_str, const char a_delim)
{
    char** result    = 0;
    size_t count     = 0;
    char* tmp        = a_str;
    char* last_comma = 0;
    char delim[2];
    delim[0] = a_delim;
    delim[1] = 0;

    /* Count how many elements will be extracted. */
    while (*tmp)
    {
        if (a_delim == *tmp)
        {
            count++;
            last_comma = tmp;
        }
        tmp++;
    }

    /* Add space for trailing token. */
    count += last_comma < (a_str + strlen(a_str) - 1);

    /* Add space for terminating null string so caller
       knows where the list of returned strings ends. */
    count++;

    result = malloc(sizeof(char*) * count);

    if (result)
    {
        size_t idx  = 0;
        char* token = strtok(a_str, delim);

        while (token)
        {
            assert(idx < count);
            *(result + idx++) = strdup(token);
            token = strtok(0, delim);
        }
        assert(idx == count - 1);
        *(result + idx) = 0;
    }

    return result;
}
