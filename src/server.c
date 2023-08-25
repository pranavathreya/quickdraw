#include <stdio.h>
#include <stdlib.h> 
#include <netdb.h>
#include <sys/socket.h>

#undef CLIENT

#include "networkTools.h"
#include "physics.h"

void serveClients(int sfd)
{
	char buf[MSG_SIZE], response[MSG_SIZE];
	struct sockaddr peerAddr;
	socklen_t peerAddrLen;
	int bytesRead, s;
	NameInfo nameInfo;
	Player player = {};
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
					&peerAddr, &peerAddrLen);
		
		if (bytesRead != MSG_SIZE)
		{
			fprintf(stderr, "server: failed to receive bytes from peer.\n");
			exit(EXIT_FAILURE);
		}
		char peerHost[NI_MAXHOST], peerService[NI_MAXSERV];
		s = getnameinfo(&peerAddr, peerAddrLen, 
				peerHost, NI_MAXHOST,
				peerService, NI_MAXSERV, 0);
		if (s!=0) {
			fprintf(stderr, "getnameinfo: %s\n", gai_strerror(s));
			exit(EXIT_FAILURE);
		}

		decodeClientState(buf, &clientState);
		logClientState(&clientState);

		updatePhysics(clientState.player, clientState.istate,
			       clientState.deltaTime);

		encodeClientState(response, MSG_SIZE, &clientState);

		if (sendto(sfd, response, MSG_SIZE, 0,
				&peerAddr, peerAddrLen)!=MSG_SIZE) {
			fprintf(stderr, "server: failed to send bytes to peer\n");
			exit(EXIT_FAILURE);
		}
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
	
	sfd = bindToAddress(argv[1], argv[2]);

	serveClients(sfd);
}



