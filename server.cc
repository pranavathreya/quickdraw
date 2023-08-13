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

void processClientMessage(char* message,
		ClientState* clientStatePtr)
{
	char* messageArray[MSG_FLD_CNT];
	char *p, currentField[MSG_SIZE];

	for (int i=0; i < MSG_FLD_CNT; ++i) {
		p = currentField;
		memset(p, '\0', MSG_SIZE);
		while ((*p++ = *message++) != '\n')
			;
		*(p-1) = '\0';
		strncpy(messageArray[i], 
				currentField, MSG_SIZE);
		fprintf(stderr, "messageArray[%d]: %s\n",
				i, messageArray[i]);
	}

	clientStatePtr->player.speed.x = atof(messageArray[0]);
	clientStatePtr->player.speed.y = atof(messageArray[1]);
	clientStatePtr->player.position.x = atof(messageArray[2]);
	clientStatePtr->player.position.y = atof(messageArray[3]);
	clientStatePtr->player.playerAngle = atof(messageArray[4]);
}


void serveClients(int sfd)
{
	char buf[MSG_SIZE];
	struct sockaddr peerAddr;
	socklen_t peerAddrLen;
	int bytesRead;
	ClientState clientState;

	for(;;)
	{
		memset(buf, 0, MSG_SIZE);
		peerAddrLen = sizeof(peerAddr);
		bytesRead = recvfrom(sfd, buf, MSG_SIZE, 0,
					&peerAddr, &peerAddrLen);
		if (bytesRead != MSG_SIZE)
		{
			fprintf(stderr, "server: failed to receive bytes from peer.\n");
			exit(EXIT_FAILURE);
		}
		char peerHost[NI_MAXHOST], peerService[NI_MAXSERV];
		getnameinfo(&peerAddr, peerAddrLen, 
				peerHost, NI_MAXHOST,
				peerService, NI_MAXSERV, 0);
		processClientMessage(buf, &clientState);
		fprintf(stderr, "server: received %s from %s:%s\n", buf, peerHost, peerService);
		fprintf(stderr, "server: received client state:\n"
				"player.speed.x:     %f\n"
				"player.speed.y:     %f\n"
				"player.position.x:  %f\n"
				"player.position.y:  %f\n"
				"player.playerAngle: %f\n",
				clientState.player.speed.x, clientState.player.speed.y,
				clientState.player.position.x, clientState.player.position.y,
				clientState.player.playerAngle);
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



