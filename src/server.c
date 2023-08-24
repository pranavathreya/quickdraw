#include <stdio.h>
#include <stdlib.h> 
#include <netdb.h>
#include <sys/socket.h>
#include "networkTools.h"
#include "physics.h"

void serveClients(int sfd)
{
	char buf[MSG_SIZE], response[MSG_SIZE];
	struct sockaddr_storage peerAddr;
	socklen_t peerAddrLen;
	int bytesRead, s;
	NameInfo nameInfo;
	Player player;
	InputState istate = inputstate_new();
	ClientState clientState = {
		&player,
		&istate,
		0
	};

	for(;;)
	{
		memset(buf, 0, MSG_SIZE);
		peerAddrLen = sizeof(peerAddr);
		bytesRead = recvfrom(sfd, buf, MSG_SIZE, 0,
			       (struct sockaddr*) &peerAddr, &peerAddrLen);
		HANDLE_R_ERR(bytesRead, MSG_SIZE);
		LOG_R_BUF(bytesRead, buf);

		decodeClientState(buf, &clientState);
		updatePhysics(clientState.player, clientState.istate, clientState.deltaTime);
		encodeClientState(response, MSG_SIZE, &clientState);
		
		s = sendto(sfd, response, MSG_SIZE, 0,
			       	(struct sockaddr*) &peerAddr, peerAddrLen);
		HANDLE_W_ERR(s, MSG_SIZE);	
		LOG_W_BUF(s, response);
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
	
	sfd = bindOrConnectToAddress(argv[1], argv[2], 1);

	serveClients(sfd);
}



