#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <signal.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/time.h>

#define LENGTH 512
#define MYTCP_PORT 80

void tv_sub(struct  timeval *out, struct timeval *in);

int main(int argc, char *argv[])
{
	
	int sockfd;
	int rcv_bytes = 0;
	float time_inv = 0.0;
	float time_file = 0.0;	

	struct sockaddr_in serv_addr;	
	struct timeval sendt, recvt;
	struct timeval startTrans, endTrans;

	gettimeofday(&sendt, NULL);

	sockfd = socket(AF_INET, SOCK_STREAM, 0);	
	if (sockfd < 0 )
	{
		perror("ERROR: Socket Failed!\n");
		exit(EXIT_FAILURE);
	}
	
	memset(&serv_addr, '0', sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(MYTCP_PORT);

	if(inet_pton(AF_INET, argv[1], &serv_addr.sin_addr) < 0)
	{
		perror("ERROR: Invalid Address or Address is in use!\n");
		exit(EXIT_FAILURE);
	}

	bzero(&(serv_addr.sin_zero), 8);

	
	if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr)) < 0)
	{
		perror("ERROR: Connection Failed!\n");
		exit(EXIT_FAILURE);
	}

	/*Get connection time*/
	gettimeofday(&recvt, NULL);
	tv_sub(&recvt, &sendt);
	time_inv = (recvt.tv_sec)*1000.0 + (recvt.tv_usec)/1000.0;
	printf("Connection time: %.3f ms\n", time_inv);
	printf("Connected Established with the server.\n");

	while(1)
	{
		char fname[1000];
		char *rcv_buffer;
		rcv_buffer = (char *) malloc(LENGTH*sizeof(char));
		
		printf("Enter the Filename or Type 'q' to exit\n");
	
		scanf ("%s", fname);
		gettimeofday(&startTrans, NULL);
		send(sockfd , fname, strlen(fname), 0);
		
		if(fname[0] == 'q')
		{
			close(sockfd);
			exit(0);
		}
		
		FILE *fp;

		printf("File Name: %s\n", fname);
		printf("Receiving file...\n");
		fp = fopen(fname, "w");
		
		if(fp == NULL)
		{
			printf("ERROR: File %s Cannot be opened.\n", fname);
			exit(EXIT_FAILURE);
		}
		
		memset(rcv_buffer, '0', sizeof(rcv_buffer));
		
		int check =0;
		
		while((rcv_bytes = read(sockfd, rcv_buffer, LENGTH)) > 0)
		{	 
			fflush(stdout);
      check = fwrite(rcv_buffer, sizeof(char),rcv_bytes,fp);
      
      if(check < rcv_bytes)
			{
				perror("ERROR: File write failed!\n");
				exit(EXIT_FAILURE);
			}
			
			memset(rcv_buffer,'0',sizeof(rcv_buffer));
			
			if (rcv_bytes < LENGTH)
			{
				printf("File recieved successfully!\n");
				
				/*Get File + Connection time*/
				gettimeofday(&endTrans, NULL);
				tv_sub(&endTrans, &startTrans);
				time_file = (endTrans.tv_sec)*1000.0 + (endTrans.tv_usec)/1000.0;
				printf("File Transfer Time: %.3f ms\n", time_file);
				break;
			}
		}

		fseek(fp, 0, SEEK_END);
		long lsize = ftell(fp);
		rewind(fp);
		printf("The file size is %d bytes\n\n", (int)lsize);
		free(rcv_buffer);
		fclose(fp);
	}
	return (0);
}

void tv_sub(struct  timeval *out, struct timeval *in)
{
	if ((out->tv_usec -= in->tv_usec) <0)
	{
		--out ->tv_sec;
		out ->tv_usec += 1000000;
	}
	out->tv_sec -= in->tv_sec;
}
