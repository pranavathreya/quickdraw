#include <stdio.h>
#include <stdlib.h> 
#include <netdb.h>
#include <sys/socket.h>
#include "networkTools.h"
#include "variables.h"

#define MSG_FLD_CNT 17

typedef struct ClientState
{
	Player player;
	InputState istate;
	float deltaTime; 

	
} ClientState;

void processClientMessage(char* message,
		ClientState* clientStatePtr)
{
	char* messageArray[MSG_FLD_CNT];
	char *p, currentField[MSG_SIZE];
	char *endptr;

	for (int i=0; i < MSG_FLD_CNT; ++i) {
		p = currentField;
		memset(p, '\0', MSG_SIZE);
		messageArray[i] = (char*) malloc(MSG_SIZE);
		while ((*p++ = *message++) != '\n')
			;
		*(p-1) = '\0';
		
		strncpy(messageArray[i], 
				currentField, MSG_SIZE);
	}
	clientStatePtr->player.speed.x = 	atof(messageArray[0]);
	clientStatePtr->player.speed.y = 	atof(messageArray[1]);
	clientStatePtr->player.position.x = 	atof(messageArray[2]);
	clientStatePtr->player.position.y = 	atof(messageArray[3]);
	clientStatePtr->player.playerAngle = 	atof(messageArray[4]);
	clientStatePtr->istate.q = 		atoi(messageArray[5]);
	clientStatePtr->istate.forward = 	atoi(messageArray[6]);
	clientStatePtr->istate.left = 		atoi(messageArray[7]);
	clientStatePtr->istate.back = 		atoi(messageArray[8]);
	clientStatePtr->istate.right = 		atoi(messageArray[9]);
	clientStatePtr->istate.strafeLeft = 	atoi(messageArray[10]);
	clientStatePtr->istate.strafeRight = 	atoi(messageArray[11]);
	clientStatePtr->istate.mouseX = 	strtoul(messageArray[12], &endptr, 10);
	clientStatePtr->istate.mouseY = 	strtoul(messageArray[13], &endptr, 10);
	clientStatePtr->istate.mouseDX = 	strtol(messageArray[14], &endptr, 10);
	clientStatePtr->istate.mouseDY = 	strtol(messageArray[15], &endptr, 10);
	clientStatePtr->deltaTime = 		atof(messageArray[16]);
	for (int i=0; i < MSG_FLD_CNT; ++i)
		free(messageArray[i]);
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
		fprintf(stderr, "server: received client state:\n"
				"player.speed.x:     %f\n"
				"player.speed.y:     %f\n"
				"player.position.x:  %f\n"
				"player.position.y:  %f\n"
				"player.playerAngle: %f\n"
				"istate.q: 	     %u\n"
				"istate.forward:     %u\n"
				"istate.left:	     %u\n"
				"istate.back:        %u\n"
				"istate.right:       %u\n"
				"istate.strafeLeft:  %u\n"
				"istate.strafeRight: %u\n"
				"istate.mouseX:      %u\n"
				"istate.mouseY:      %u\n"
				"istate.mouseDX:     %d\n"
				"istate.mouseDY:     %d\n"
				"deltaTime: 	     %f\n", 
			clientState.player.speed.x, clientState.player.speed.y,
			clientState.player.position.x, clientState.player.position.y,
			clientState.player.playerAngle,
			clientState.istate.q,
			clientState.istate.forward,
			clientState.istate.left,
			clientState.istate.back,
			clientState.istate.right,
			clientState.istate.strafeLeft,
			clientState.istate.strafeRight,
			clientState.istate.mouseX,
			clientState.istate.mouseY,
			clientState.istate.mouseDX,
			clientState.istate.mouseDY,
			clientState.deltaTime);
	}	
}

int main(int argc, char** argv)
{
	if (argc != 3)
	{
		fprintf(stderr, "Usage: %s host port\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	int sfd;
	
	fprintf(stderr, "no seg faul before this: \n");
	sfd = bindOrConnectToAddress(argv[1], argv[2], 1);

	serveClients(sfd);
}



