#include <stdio.h>
#include <stdlib.h> 
#include <netdb.h>
#include <sys/socket.h>
#include "networkTools.h"
#include "variables.h"

#define MSG_FLD_CNT 5

typedef struct ClientState
{
	Player player;
} ClientState;

//ClientState processClientMessage(char* message)
//{
//	char* messageFields[MSG_FLD_CNT];
//	ClientState clientState;
//	while (*message!='\0')
//

void serveClients(int sfd)
{
	char buf[MSG_SIZE];
	struct sockaddr peerAddr;
	socklen_t peerAddrLen;

	for(;;)
	{
		memset(buf, 0, MSG_SIZE);
		peerAddrLen = sizeof(peerAddr);
		if (recvfrom(sfd, buf, MSG_SIZE, 0,
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
	if (argc != 3)
	{
		fprintf(stderr, "Usage: %s port\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	int sfd;
	
	sfd = bindOrConnectToAddress(argv[1], argv[2], 1);

	serveClients(sfd);
}



