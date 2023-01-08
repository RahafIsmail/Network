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


int USER;

//Function to check user credentials
bool checkUserCredentials(char* received)
{
	int  check=0;
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

//Function to list the messages in the user's directory
bool listMessages(int fd){
	//Change to the user's directory
	char subDir[100];
	sprintf(subDir, "%d", USER); 
	if (chdir(subDir) != 0){
		printf("ERROR WHILE CHANGING THE DIRECTORY!\n");
		return false;
	}
	
	int count = 1;
	bool checkList = false;
	
	//Open the current directory
	DIR *dir_pointer;
	struct dirent *dir_info;
	dir_pointer = opendir(".");
	
	if (dir_pointer){
		//Create an array to save the data
		int data[100][4];
		int i = 0;
		
		while ((dir_info = readdir(dir_pointer)) != NULL){
			//List all .msg files
			if (strstr(dir_info->d_name, ".msg") != NULL && dir_info->d_name[0] != '.' && dir_info->d_type == DT_REG){
				checkList = true;
				//Split the filename
				char *token;
				token = strtok(dir_info->d_name, "_");
				
				int unique_id = count;
				int from_user_id;
				int time_stamp;
				int txt_size;
				
				int j = 1;
				while (token != NULL){
					if (j == 1)
						time_stamp = atoi(token);
					else if (j == 2)
						from_user_id = atoi(token);
					else if (j == 3)
						txt_size = atoi(token);
					
					token = strtok(NULL, "_");
					j++;
				}
				
				//Save the data to the array
				data[i][0] = unique_id;
				data[i][1] = from_user_id;
				data[i][2] = time_stamp;
				data[i][3] = txt_size;
				
				count++;
				i++;
			}
		}
		closedir(dir_pointer);
		if (checkList){ //To make sure it is not empty
			//Sort the data based on time_stamp
			int j;
			for (i = 0; i < count-1; i++){
				for (j = i+1; j < count-1; j++){
					if (data[i][2] > data[j][2]){
						int temp[4];
						memcpy(temp, data[i], sizeof(temp));
						memcpy(data[i], data[j], sizeof(temp));
						memcpy(data[j], temp, sizeof(temp));
					}
				}
			}
			
			//Store the result into a string
			char list_of_messages[1000] = "";
			char x[100] = "+OK listing follows. \n";
			strcat(list_of_messages, x);
			for (i = 0; i < count-1; i++){
				char to_append[30];
				sprintf(to_append, "%d %d %d %d\n", data[i][0], data[i][1], data[i][2], data[i][3]);
				strcat(list_of_messages, to_append);
			}
			
			int send_count = send(fd, list_of_messages, strlen(list_of_messages), 0);//Send it
			if (send_count < 0){
				printf("ERROR WHILE SENDING DATA!\n");
				return false;
			}
			return true;
		}
	}
	
	if (!checkList){
		char list_of_messages[1000] = "-ERR there are no message";
		int send_count = send(fd, list_of_messages, strlen(list_of_messages), 0);//Send it
		if (send_count < 0){
			printf("ERROR WHILE SENDING DATA!\n");
			return false;
		}
	}
	return false;
}


void showMenu(int fd){
	char menu[256] = "\n\nPlease choose your option:\n1) Read/Delete Messages\n2) Write Message to User\n3) Change config parameters\n4) Quit\nOption->";
	send(fd, menu, strlen(menu), 0);
}

bool readMessages(int fd, char* received){
	int t;
	sscanf(received, "RET %d", & t);
	char user[100];
	sprintf(user, "%d", USER); //int to string
	bool z = false;
	chdir(user);
	int no = 1;
	DIR * a;
	struct dirent * directory;
	a = opendir(".");
	if (a){
	
	  //create an array to save the data
	
	  int myArray[100][4];
	  int i = 0;
	

	  while ((directory = readdir(a)) != NULL)
	
	  {
	
	    //list all .msg files
	
	    if (strstr(directory -> d_name, ".msg") != NULL && directory -> d_name[0] != '.' && directory -> d_type == DT_REG)
	
	    {
	      //split the filename
	      char * token;
	      token = strtok(directory -> d_name, "_");
	
	      int unique_id = no;
	      int from_user_id;
	      int time_stamp;
	      int txt_size;
	      int j = 1;
	      z = true;
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
	

	      myArray[i][0] = unique_id;
	      myArray[i][1] = from_user_id;
	      myArray[i][2] = time_stamp;
	      myArray[i][3] = txt_size;
	      no++;
	      i++;
	      
	    }
	
	}
		if (z) 
	{
	  //sort the data based on time_stamp
	
	  int j;
	
	  for (i = 0; i < no - 1; i++)
	
	  {
	
	    for (j = i + 1; j < no - 1; j++)
	
	    {
	
	      if (myArray[i][2] > myArray[j][2])
	
	      {
	
	        int temp[4];
	        memcpy(temp, myArray[i], sizeof(temp));
	        memcpy(myArray[i], myArray[j], sizeof(temp));
	        memcpy(myArray[j], temp, sizeof(temp));
	      }
	    }
	  }
	  
	  //display the result// hide it later
		for (i = 0; i < no-1; i++)
		{
			if (t == i+1) //when you arrive to the index, read and send it
			{
				//read the content of the file
				char file_content[1000]="";
				
				char buf[1000] = "";
				int size = 0;
				
				//open the file
				sprintf(buf, "%d_%d_%d.msg", myArray[i][2], myArray[i][1], myArray[i][3]);
				FILE *fptr;
				fptr = fopen(buf, "rb");
				
				if (fptr == NULL)
				{
					printf("Error!");
					return false;
				}
				else
				{
					//read the file
					fseek(fptr, 0, SEEK_END);
					size = ftell(fptr);
					fseek(fptr, 0, SEEK_SET);
					fread(file_content, 1, size, fptr);
					
					char x[1000] = "+OK message follows \n";
					strncat(x, file_content, strlen(file_content));
					
					
					send(fd, x, strlen(x), 0);
					
					fclose(fptr);
					return true;
				}

			}
		
		}
	  
	}
	
}
	
}








void do_job(int fd) {
int sent; 
int  length = DEFAULT_BUFLEN; 

bool auth=false;




char received[DEFAULT_BUFLEN],bmsg[DEFAULT_BUFLEN]; 


do {
	if(auth)
	showMenu(fd);
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
        if (memcmp(received, "LIST", 4) == 0){
        	listMessages(fd);
		}
		//////////////////////////////////////////////////////////////////////////////////////////////////////////
		if (memcmp(received, "RET", 3) == 0){
        	if (!readMessages(fd, received))
        	{
        		char notPass[1000] = "-ERR no such message.";
				send(fd, notPass, strlen(notPass), 0);
			}
		}
	
		
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
