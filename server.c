/*
  This code represent basic Concurrent Echo Server for linux platform. It's used to teach how basic server  running.
For compile code : # gcc concurrent.c -o concurrent
  -----------------------Copyright Devrim Seral---------------------------------
*/
/* C header files */
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Socket API headers */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/* Definations */
#define DEFAULT_BUFLEN 512


void do_job(int fd) {
int length,rcnt;
char recvbuf[DEFAULT_BUFLEN],bmsg[DEFAULT_BUFLEN];
int  recvbuflen = DEFAULT_BUFLEN;

    // Receive until the peer shuts down the connection
    do {
        rcnt = recv(fd, recvbuf, recvbuflen, 0);
        if (rcnt > 0) {
            printf("Bytes received: %d\n", rcnt);

        // Echo the buffer back to the sender
        rcnt = send( fd, recvbuf, rcnt, 0 );
            if (rcnt < 0) {
                printf("Send failed:\n");
                close(fd);
                break;
            }
            printf("Bytes sent: %d\n", rcnt);

        }
        else if (rcnt == 0)
            printf("Connection closing...\n");
        else  {
            printf("Receive failed:\n");
            close(fd);
            break;
        }
    } while (rcnt > 0);
}



int main(int argc, char *argv[])
{
int server, client;
struct sockaddr_in local_addr;
struct sockaddr_in remote_addr;
int length,fd,rcnt,optval;
pid_t pid;

	int PNum;
	PNum = atoi(argv[4]);

        if (chdir(argv[2]) == 0) {
            printf("New Directory = ", argv[2]);
        } else {
            printf("Error in changing the Directory");
        }


/* Open socket descriptor */
if ((server = socket( AF_INET, SOCK_STREAM, 0)) < 0 ) { 
    perror("Can't create socket!");
    return(1);
}


/* Fill local and remote address structure with zero */
memset( &local_addr, 0, sizeof(local_addr) );
memset( &remote_addr, 0, sizeof(remote_addr) );

/* Set values to local_addr structure */
local_addr.sin_family = AF_INET;
local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
local_addr.sin_port = htons(PNum); 

// set SO_REUSEADDR on a socket to true (1):
optval = 1;
setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);

if ( bind( server, (struct sockaddr *)&local_addr, sizeof(local_addr) ) < 0 )
{
    /* could not start server */
    perror("Bind error");
    return(1);
}

if ( listen( server, SOMAXCONN ) < 0 ) {
        perror("listen");
        exit(1);
}

printf("Concurrent  socket server now starting on port %d\n",PNum);
printf("Wait for connection\n");


	FILE *fp;
	char str[255];
	fp = fopen("user.txt", "r");  
	if (fp == NULL) 
	{
		printf("Could not open file\n");
		return 0;
	}
	while (fgets(str, 255, fp) != NULL) 
	{
		int ID;
		char PASS[255];
		sscanf(str, "%d:%s", &ID, PASS);
		printf("ID = %d\n", ID);
		printf("PASSWORD = %s\n", PASS);
	}
	fclose(fp);


while(1) {  // main accept() loop
    length = sizeof remote_addr;
    if ((fd = accept(server, (struct sockaddr *)&remote_addr, 
          &length)) == -1) {
          perror("Accept Problem!");
          continue;
    }

    printf("Server: got connection from %s\n", 
            inet_ntoa(remote_addr.sin_addr));

    /* If fork create Child, take control over child and close on server side */
    if ((pid=fork()) == 0) {
        close(server);
        do_job(fd);
        printf("Child finished their job!\n");
        close(fd);
        exit(0);
    }

}

// Final Cleanup
close(server);

}
