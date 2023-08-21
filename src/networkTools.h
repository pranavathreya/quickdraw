#ifndef NETWORK_TOOLS_HDR
#define NETWORK_TOOLS_HDR

#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "variables.h"

#define MSG_FLD_CNT 17

typedef struct ClientState
{
	Player* player;
	InputState* istate;
	float deltaTime; 

	
} ClientState;

void encodeClientState(char* buf, int bufSize,
	       	ClientState* clientState)
{
	snprintf(buf, bufSize,
		"%f\n%f\n%f\n%f\n%f\n%d\n%d\n%d\n%d"
		"\n%d\n%d\n%d\n%u\n%u\n%d\n%d\n%f\n", 
		clientState->player->speed.x,
		clientState->player->speed.y,
		clientState->player->position.x,
		clientState->player->position.y,
		clientState->player->playerAngle,
		clientState->istate->q,
		clientState->istate->forward,
		clientState->istate->left,
		clientState->istate->back,
		clientState->istate->right,
		clientState->istate->strafeLeft,
		clientState->istate->strafeRight,
		clientState->istate->mouseX,
		clientState->istate->mouseY,
		clientState->istate->mouseDX,
		clientState->istate->mouseDY,
		clientState->deltaTime);
}

void populateClientState(char* messageArray[], ClientState* clientStatePtr)
{
	char *endptr;

	clientStatePtr->player->speed.x = 	atof(messageArray[0]);
	clientStatePtr->player->speed.y = 	atof(messageArray[1]);
	clientStatePtr->player->position.x = 	atof(messageArray[2]);
	clientStatePtr->player->position.y = 	atof(messageArray[3]);
	clientStatePtr->player->playerAngle = 	atof(messageArray[4]);
	clientStatePtr->istate->q = 		atoi(messageArray[5]);
	clientStatePtr->istate->forward = 	atoi(messageArray[6]);
	clientStatePtr->istate->left = 		atoi(messageArray[7]);
	clientStatePtr->istate->back = 		atoi(messageArray[8]);
	clientStatePtr->istate->right = 	atoi(messageArray[9]);
	clientStatePtr->istate->strafeLeft = 	atoi(messageArray[10]);
	clientStatePtr->istate->strafeRight = 	atoi(messageArray[11]);
	clientStatePtr->istate->mouseX = 	strtoul(messageArray[12], &endptr, 10);
	clientStatePtr->istate->mouseY = 	strtoul(messageArray[13], &endptr, 10);
	clientStatePtr->istate->mouseDX = 	strtol(messageArray[14], &endptr, 10);
	clientStatePtr->istate->mouseDY = 	strtol(messageArray[15], &endptr, 10);
	clientStatePtr->deltaTime = 		atof(messageArray[16]);
}

void decodeClientState(char* message,
		ClientState* clientStatePtr)
{
	char* messageArray[MSG_FLD_CNT];
	char *p, currentField[MSG_SIZE];

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
	
	populateClientState(messageArray, clientStatePtr);

	for (int i=0; i < MSG_FLD_CNT; ++i)
		free(messageArray[i]);
}

void logClientState(const ClientState* clientState)
{
		fprintf(stderr, "server: received client state:\n"
				"player.speed.x:     %f\nplayer.speed.y:     %f\n"
				"player.position.x:  %f\nplayer.position.y:  %f\n"
				"player.playerAngle: %f\nistate.q: 	     %u\n"
				"istate.forward:     %u\nistate.left:	     %u\n"
				"istate.back:        %u\nistate.right:       %u\n"
				"istate.strafeLeft:  %u\nistate.strafeRight: %u\n"
				"istate.mouseX:      %u\nistate.mouseY:      %u\n"
				"istate.mouseDX:     %d\nistate.mouseDY:     %d\n"
				"deltaTime: 	     %f\n\n", 
			clientState->player->speed.x, 		clientState->player->speed.y,
			clientState->player->position.x, 	clientState->player->position.y,
			clientState->player->playerAngle,	clientState->istate->q,
			clientState->istate->forward, 		clientState->istate->left,
			clientState->istate->back, 		clientState->istate->right,
			clientState->istate->strafeLeft, 	clientState->istate->strafeRight,
			clientState->istate->mouseX, 		clientState->istate->mouseY,
			clientState->istate->mouseDX, 		clientState->istate->mouseDY,
			clientState->deltaTime);
}

void getAddressInfo(char* host, char* port,
		struct addrinfo** result)
{	
	int s;
	struct addrinfo hints;
	
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = 0;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_canonname = NULL;
	hints.ai_addr = NULL;
	hints.ai_addrlen = 0;
	
	s = getaddrinfo(host, port, &hints,
			result);
	if (s == -1)
	{
		fprintf(stderr, "getAddressInfo: getaddrinfo failed: %s\n",
			gai_strerror(s));
		exit(EXIT_FAILURE);
	}
}

int getSocket(int _bind, struct addrinfo *result)
{
	struct addrinfo *rp;
	int sfd;
	
	for (rp=result; rp!=NULL; rp=rp->ai_next)
	{
		sfd = socket(rp->ai_family, rp->ai_socktype,
			       	rp->ai_protocol);
		
		if (sfd == -1)
			continue;

		int err;
		if (_bind)
			err = bind(sfd, rp->ai_addr,
				       rp->ai_addrlen);
		else
			err = connect(sfd, rp->ai_addr,
					rp->ai_addrlen);
		if (!err)
			break;
	}

	if (rp == NULL)
	{
		fprintf(stderr, "server: Failed to bind.\n");
		exit(EXIT_FAILURE);
	}

	char finalHost[NI_MAXHOST], serv[NI_MAXSERV];

	getnameinfo(rp->ai_addr, rp->ai_addrlen,
			finalHost, NI_MAXHOST,
			serv, NI_MAXSERV, 0);
	fprintf(stderr, "getServerFileDescriptor: successfully %s to: %s:%s\n",
		       _bind ? "bound" : "connected",	finalHost, serv);

	return sfd;
	
}

int bindOrConnectToAddress(char* host, char* port,
	       	int _bind)
{
	struct addrinfo *result; 
	int sfd;

	getAddressInfo(host, port, &result);	
	sfd = getSocket(_bind, result);	
	freeaddrinfo(result);
	
	return sfd;
}
#endif
