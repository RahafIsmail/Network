/*
  This code represent basic Concurrent Echo Server for linux platform. It's used to teach how basic server  running.
For compile code : # gcc concurrent.c -o concurrent
  -----------------------Copyright Devrim Seral---------------------------------
*/
/* C header files */
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <stdbool.h>
/* Socket API headers */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
/* Definations */
#define DEFAULT_BUFLEN 512




//Function to check user credentials
bool checkUserCredentials(char* received)
{
	int USER, check=0;
	char PASS[20];
	char line[50], col1[20], col2[20];
	
	//Check if the received string starts with "USER"
	if (memcmp(received, "USER", 4) == 0){
		//Read the user credentials
		sscanf(received, "USER %d %[^\n]", &USER, PASS);
		
		// Read the user.txt file
		FILE *fp = fopen("user.txt", "r");
	    if (fp == NULL)
		{
		    printf("ERROR WHILE OPEN THE FILE!\n");
		    return false;
		}
			
		//Check the user credentials with the ones in the file
		while (fgets(line, sizeof(line), fp) != NULL)
	    {
	        sscanf(line, "%[^:]:%s", col1, col2);
	        int ID = atoi(col1); 
	        if (USER == ID)
	        {
	            if (strcmp(PASS, col2) == 0)
	            {
	                check = 1;
	            }
	        }
	    }
	    fclose(fp);
		
		//If the user credentials are valid, return true
	    if (check){
	        return true;
	    }
		
		//Else, return false
	    else{
	        return false;
	    }
	}
	
	//If the received string doesn't start with "USER", return false
	else{
		return false;
	}
}




void do_job(int fd) {
int sent; 
int  length = DEFAULT_BUFLEN; 
int  USER; 
char PASS[100]; 
char line[256];
char col1[256], col2[256];
int check = 0; 

bool auth=false;




char received[DEFAULT_BUFLEN],bmsg[DEFAULT_BUFLEN]; 

 // Receive until the peer shuts down the connection
do {
    sent = recv(fd, received, length, 0);
        
		if (memcmp(received, "USER", 4) == 0){
			auth = checkUserCredentials(received);
			if (auth)
			{
				char pass[100] = "+OK UserID and password okay go ahead. ";
				sent = send(fd, pass, strlen(pass), 0);
			}
			else
			{
				char pass[100] = "-ERR Either UserID or Password is wrong. ";
				sent = send(fd, pass, strlen(pass), 0);
			}
			
		}
/////////////////////////////listt


        if (memcmp(received, "LIST", 4) == 0) //done listing perfectly with date order
        {
        	printf("list is working");
        	
        	//change to the subdirectory
        	
        	char sub_directory[100];//subD
			int count = 1;
			DIR *d;
		struct dirent *dir;
		d = opendir(".");
		//create an array to save the data
			int data[100][4];
			int i = 0;
			char *token;
					token = strtok(dir->d_name, "_");
					//getdir
					int unique_id = count;
					int from_user_id;
					int time_stamp;
					int txt_size;
					
					
        	sprintf(sub_directory, "%d", USER); //int to string
			printf("%s\n you are in: ", sub_directory);
			
			sprintf(sub_directory, "%d", USER);
        	chdir(sub_directory);
        	
	
		//open the current directory
		
		if (d)
		{
			while ((dir = readdir(d)) != NULL)
			{
				
				//list all .msg files
				if (strstr(dir->d_name, ".msg") != NULL && dir->d_name[0] != '.' && dir->d_type == DT_REG)
				{
					check = true;
					//split the filename
						char *token;
					token = strtok(dir->d_name, "_");
					
					int j = 1;
					while (token != NULL) 
					{
						if (j == 1)
							time_stamp = atoi(token);
						else if (j == 2)
							from_user_id = atoi(token);
						else if (j == 3)
							txt_size = atoi(token);
						
						token = strtok(NULL, "_");
						j++;
					}
					
					//save the data to the array
					data[i][0] = unique_id;
					data[i][1] = from_user_id;
					data[i][2] = time_stamp;
					data[i][3] = txt_size;
					
					count++;
					i++;
				}
			}
					closedir(d);
				if (check) //to make sure it is not empty
				{
					//sort the data based on time_stamp
					int j;
					for (i = 0; i < count-1; i++)
					{
						for (j = i+1; j < count-1; j++)
						{
							if (data[i][2] > data[j][2])
							{
								int temp[4];
								memcpy(temp, data[i], sizeof(temp));
								memcpy(data[i], data[j], sizeof(temp));
								memcpy(data[j], temp, sizeof(temp));
							}
						}
					}
					
					//display the result// hide it later
					for (i = 0; i < count-1; i++)
					{
						printf("%d %d %d %d\n", data[i][0], data[i][1], data[i][2], data[i][3]);
					}
					
					
					//store the result into a string
					char list[1000] = "";
					char x[100] = "+OK listing follows. \n";
					strcat(list, x);
					for (i = 0; i < count-1; i++)
					{
						char to_append[30];
						sprintf(to_append, "%d %d %d %d\n", data[i][0], data[i][1], data[i][2], data[i][3]);
						strcat(list, to_append);
					}
					
					sent = send(fd, list, strlen(list), 0);// send it
				}
		
			}
			if (!check)
			{
				char list[1000] = "-ERR there are no message";
				sent = send(fd, list, strlen(list), 0);// send it
			}
		}//LIST
		//////////////////////////////////////////////////////////////////////////////////////////////////////////
		
	
/*
	
		 if (memcmp(received, "LIST", 4) == 0) {
			 
			int res;
			char dire[1024]; 
			int i=1;
		    int number =1;
			DIR *dir;
			struct dirent *s;
			int idd;
			int time;
			int octets;
        	char SubDirectory[200];
			char *file;
        	
        	res = chdir(SubDirectory);
  
		    getdire(dire, sizeof(dire)); 
		    printf("Current Directory: %s\n", dire); 
		    dir = opendir(dire);  


			while( (s = Direct( dir )) != NULL ){  
			
			    file = strdup(s->d_name); // copy the file to a variable "file"

			    sscanf(file, "%d_%d_%d.msg", &idd, &time, &octets);
			
			    printf("%d %d %d %d\n", number, idd, time, octets);
			
			    free(file);
			    number = ++i;
		}}
		*/
		
		/////////////////////////////////////////////////////////////////////////////////////
		
		
        if (sent > 0) {
            
        }
        else if (sent == 0){
		printf("Connection closing...\n");}
        else  {
            printf("Failed! \n");
            close(fd);
            break;
}}
     while (sent > 0);
}

int main(int argc, char *argv[])
{
int server, client;
struct sockaddr_in local_addr;
struct sockaddr_in remote_addr;
int length,fd,sent,optval;
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

/*
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
*/

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
