#include <stdio.h>
#include <stdlib.h> 
#include <netdb.h>
#include <sys/socket.h>
#include "networkTools.h"

#define BUF_SIZE 500

void serveClients(int sfd)
{
	char buf[BUF_SIZE];
	struct sockaddr peerAddr;
	socklen_t peerAddrLen;

	for(;;)
	{
		peerAddrLen = sizeof(peerAddr);
		if (recvfrom(sfd, buf, BUF_SIZE, 0,
				&peerAddr, &peerAddrLen) == -1)
		{
			fprintf(stderr, "server: failed to receive bytes from peer.\n");
			exit(EXIT_FAILURE);
		}

		char peerHost[NI_MAXHOST], peerService[NI_MAXSERV];
		getnameinfo(&peerAddr, peerAddrLen, 
				peerHost, NI_MAXHOST,
				peerService, NI_MAXSERV, 0);
		fprintf(stderr, "server: received %s from %s:%s\n", buf, peerHost, peerService);
	}	
}

int main(int argc, char** argv)
{
	if (argc != 2)
	{
		fprintf(stderr, "Usage: %s port\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	int sfd;

	sfd = bindOrConnectToAddress("localhost", argv[1], 1);

	serveClients(sfd);
}



