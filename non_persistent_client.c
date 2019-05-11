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

#define MYTCP_PORT 80

void tv_sub(struct  timeval *out, struct timeval *in);

int main(int argc, char *argv[])
{
	while(1)
	{
		int sockfd, rcv_bytes = 0;
		char filename[100];
		char recvBuff[1024];
		float time_inv = 0.0;
		float file_time = 0.0;
		
		struct sockaddr_in serv_addr;	
		struct timeval sendt, recvt;
		struct timeval startTrans, endTrans;
		
		memset(recvBuff, '0', sizeof(recvBuff));

		gettimeofday(&sendt, NULL);
		
		sockfd = socket(AF_INET, SOCK_STREAM, 0);
		
		if (sockfd < 0)
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

		printf("Connected Established with the server.\n");

		/*Get connection time*/
		gettimeofday(&recvt, NULL);
		tv_sub(&recvt, &sendt);
		time_inv = (recvt.tv_sec)*1000.0 + (recvt.tv_usec)/1000.0;
		printf("Connection time: %.3f ms\n", time_inv);
		
		printf("Enter the Filename or Type 'q' to exit\n");
		scanf ("%s", filename);
		
		gettimeofday(&startTrans, NULL);
		send(sockfd , filename, strlen(filename), 0);
		
		if(filename[0] == 'q')
		{
			close(sockfd);
			exit(0);
		}
		
		FILE *fp;
		printf("File Name: %s\n", filename);
		printf("Receiving file...\n");
		fp = fopen(filename, "ab");
		if(fp == NULL)
		{
			printf("ERROR: File %s Cannot be opened.\n", filename);
			exit(EXIT_FAILURE);
		}

		while((rcv_bytes = read(sockfd, recvBuff, 1024)) > 0)
		{	 
			fflush(stdout);
      fwrite(recvBuff, 1,rcv_bytes,fp);
			if (rcv_bytes < 1024)
			{
				printf("File recieved successfully!\n");
				
				/*Get File + Connection time*/
				gettimeofday(&endTrans, NULL);
				tv_sub(&endTrans, &startTrans);
				file_time = (endTrans.tv_sec)*1000.0 + (endTrans.tv_usec)/1000.0;
				printf("\nTotal Time: %.3f ms\n", file_time+time_inv);
				break;
			}
		}
		
		fseek(fp, 0, SEEK_END);
		long lsize = ftell(fp);
		rewind(fp);
		printf("The file size is %d bytes\n\n", (int)lsize);
		fclose(fp);
		close (sockfd);
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
