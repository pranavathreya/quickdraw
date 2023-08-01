#include <stdio.h>
#include <stdlib.h> 
#include <netdb.h>
#include <sys/socket.h>

#define BUF_SIZE 500

int setupServer(char* port)
{
	struct addrinfo hints;
	struct addrinfo *result, *rp;
	int s, sfd;
	
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = 0;
	hints.ai_canonname = NULL;
	hints.ai_addr = NULL;
	hints.ai_addrlen = 0;
	
	s = getaddrinfo(NULL, port, &hints,
			&result);

	if (s == -1)
	{
		fprintf(stderr, "server: getaddrinfo failed: %s\n",
			gai_strerror(s));
		exit(EXIT_FAILURE);
	}

	for (rp=result; rp!=NULL; 
			rp=rp->ai_next)
	{
		sfd = socket(rp->ai_family, rp->ai_socktype,
			       	rp->ai_protocol);
		
		if (sfd == -1)
			continue;

		if (bind(sfd, rp->ai_addr,
				       rp->ai_addrlen) == 0)
			break;	
	}

	freeaddrinfo(result);

	if (rp == NULL)
	{
		fprintf(stderr, "server: Failed to bind.\n");
		exit(EXIT_FAILURE);
	}

	char host[NI_MAXHOST], serv[NI_MAXSERV];

	getnameinfo(rp->ai_addr, rp->ai_addrlen,
			host, NI_MAXHOST,
			serv, NI_MAXSERV, 0);
	fprintf(stdout, "server: successfully bound to: %s:%s\n", host, port);
	return sfd;
}

int main(int argc, char** argv)
{
	if (argc != 2)
	{
		fprintf(stderr, "Usage: %s port\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	int sfd;
	char buf[BUF_SIZE];
	struct sockaddr peerAddress;
	size_t peerAddressLen;

	sfd = setupServer(argv[1]);
}



