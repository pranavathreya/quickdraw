#pragma once

#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "variables.h"

#define MSG_FLD_CNT 17
#define HANDLE_W_ERR(s, n)\
if (s!=n) {\
	fprintf(stderr, "partial/failed write: %d/%lu\n"\
			, s, n);\
       	exit(EXIT_FAILURE);\
}
#define HANDLE_R_ERR(s, n)\
if (s!=n) {\
	fprintf(stderr, "partial/failed read: %d/%lu\n"\
			, s, n);\
       	exit(EXIT_FAILURE);\
}

#define LOG_W_BUF(n, buf)\
fprintf(stderr, "wrote %d bytes of messageBuffer:\n%s",\
		n, buf);

#define LOG_R_BUF(n, buf)\
fprintf(stderr, "read %d bytes of messageBuffer:\n%s",\
		n, buf);


typedef struct ClientState
{
	Player* player;
	InputState* istate;
	float deltaTime; 

	
} ClientState;

typedef struct NameInfo
{
	char host[NI_MAXHOST];
	char serv[NI_MAXSERV];
} NameInfo;

void encodeClientState(char* buf, int bufSize,
	       	ClientState* clientState)
{
	int bytesRead = 0;
	memcpy(buf, clientState->player, sizeof(Player));
	memcpy(buf + sizeof(Player), clientState->istate, sizeof(InputState));
	memcpy(buf + sizeof(Player) + sizeof(InputState), &(clientState->deltaTime), sizeof(float));
}

void decodeClientState(char* message,
		ClientState* clientStatePtr)
{
	memcpy(clientStatePtr->player, message, sizeof(Player));
	memcpy(clientStatePtr->istate, message + sizeof(Player), sizeof(InputState));
	memcpy(&(clientStatePtr->deltaTime), message + sizeof(Player) + sizeof(InputState), sizeof(float));
}

void logClientState(const ClientState* clientState)
{
		fprintf(stderr, "logClientState: received client state:\n"
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

void _getNameInfo(struct sockaddr* ai_addr, socklen_t ai_addrlen,
		NameInfo* nameInfo)
{
	int s;
	s = getnameinfo(ai_addr, ai_addrlen,
			nameInfo->host, NI_MAXHOST,
			nameInfo->serv, NI_MAXSERV, 0);
	if (s!=0) {
		fprintf(stderr, "_getNameInfo: getnameinfo failed: %s\n", gai_strerror(s));
		fprintf(stderr, "sa_family: %d\nsa_data: %s\n", ai_addr->sa_family,
				ai_addr->sa_data);
		exit(EXIT_FAILURE);
	}
}

int getSocket(int _bind, struct addrinfo *result)
{
	struct addrinfo *rp;
	int sfd;
	NameInfo nameInfo;
	
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
	
	_getNameInfo(rp->ai_addr, rp->ai_addrlen, &nameInfo);
	fprintf(stderr, "getSocket: successfully %s to: %s:%s\n",
		       _bind ? "bound" : "connected",	nameInfo.host, nameInfo.serv);

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

int bindToAddress(char* host, char* port) {
	return bindOrConnectToAddress(host, port, 1);
}

int connectToAddress(char* host, char* port) {
	return bindOrConnectToAddress(host, port, 0);
}
