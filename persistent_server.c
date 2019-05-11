#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <math.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/time.h>

#define MAX_DATA 1024
#define BACKLOG 10
#define MYTCP_PORT 80

char fileName[100];
void SendFileToClient(int sockfd);

int main (int argc, char *argv[])
{

	int server_fd, conn_fd, ret;
	struct sockaddr_in addr_serv; 
	struct sockaddr_in addr_client; 
	
	socklen_t sin_size;
	//int PORT;
	int flag;
	int count = 0;
	
	if(argc != 2)
	{
		printf("ERROR: Parameters not match!\n");
		exit(0);
	}
	
	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(server_fd < 0)
	{
		perror("ERROR: Socket failed!");
		exit(EXIT_FAILURE);
	}
	
	printf("Socket successfully obtained.\n");

	flag = atoi(argv[1]);

	addr_serv.sin_family = AF_INET; 
	addr_serv.sin_addr.s_addr = htonl(INADDR_ANY); 
	addr_serv.sin_port = htons(MYTCP_PORT); 
	bzero(&(addr_serv.sin_zero), 8);
	
	ret = bind(server_fd, (struct sockaddr*)&addr_serv, sizeof(addr_serv));
	if(ret < 0)
	{
		perror("ERROR: Binding failed!");
		exit(EXIT_FAILURE);
	}
	
	printf("Connection Binded Sucessfully.\n");

	ret = listen(server_fd, BACKLOG);
	if(ret < 0)
	{
		perror("ERROR: Listening failed!");
		exit(EXIT_FAILURE);
	}
	
	printf ("Listening for the connection.\n");

	while(1) 
	{
		//check whether server run in persistent or non-persistent mode
		if((flag == 1) && (count != 1))
		{
			perror("Server running for persistent client.\n");
			sin_size = sizeof (struct sockaddr_in);
			conn_fd = accept(server_fd, (struct sockaddr *)&addr_client, &sin_size);
			count = 1;
			if (conn_fd < 0)
			{
				perror("Error in Connection\n");
				exit(EXIT_FAILURE);
			}
		}
		
		if(flag == 0)
		{
			perror("Server running for non-persistent client.\n");
			sin_size = sizeof (struct sockaddr_in);
			conn_fd = accept(server_fd, (struct sockaddr *)&addr_client, &sin_size);
			count = 1;
			if (conn_fd < 0)
			{
				perror("Error in Connection\n");
				exit(EXIT_FAILURE);
			}
		}
		
		read(conn_fd, fileName, 256);
		if (fileName[0] == 'q') 
		{
			printf("Client disconnected\n");
			close(conn_fd);
			exit(1);
		} else
		{
			SendFileToClient(conn_fd);
		}
	}
}

void SendFileToClient(int sockfd)
{
	int connfd = sockfd;

	FILE *fp = fopen(fileName,"rb");
  if(fp==NULL)
  {
		printf("ERROR: Could not open the file.");
  }   

  /* Read data from file and send it */
  while(1){
  /* First read file in chunks of 256 bytes */
  unsigned char buff[1024]={0};
  int nread = fread(buff,1,1024,fp);

  /* If read was success, send data. */
  if(nread > 0)
  {
  	write(connfd, buff, nread);
  }
  if (nread < MAX_DATA)
  {
  	if (feof(fp))
		{
  		printf("End of the file.\n");
			printf("File transfer completed.\n");
		}
    if (ferror(fp))
    	printf("Error in reading the file.\n");
     	break;
    }
	}
}
