#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

#define BUF_SIZE 500

int main(int argc, char* argv[])
{
	struct addrinfo hints;
	struct addrinfo *result, *rp;
	
	int sfd, s;
	struct sockaddr_storage peerinfo;
	socklen_t peeraddr_len;
	ssize_t nread;

	char buf[BUF_SIZE];

	if (argc!=2)
	{
		fprintf(stderr, "server: Usage: %s port\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = 0;
	hints.ai_addr = NULL;
	hints.ai_canonname = NULL;
	hints.ai_next = NULL;

	s = getaddrinfo(NULL, argv[1], &hints, &result);
	if (s != 0)
	{
		fprintf(stderr, "server: getaddrinfo failed bitch: %s\n", gai_strerror(s));
		exit(EXIT_FAILURE);
	}

	for (rp = result; rp != NULL; rp = rp->ai_next)
	{	
		sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (sfd == -1)
			continue;

		if (bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0)
			break; /* Success */
		close(sfd);
	}

	freeaddrinfo(result);
	
	if (rp == NULL)
	{
		fprintf(stderr, "server: binding failed bitch\n");
		exit(EXIT_FAILURE);
	}

	
	char host[NI_MAXHOST], service[NI_MAXSERV];
	getnameinfo((struct sockaddr*) rp->ai_addr, rp->ai_addrlen,
			host, NI_MAXHOST,
			service, NI_MAXSERV, 0);
	fprintf(stdout, "server: Successfully bound running on: %s:%s\n", host, service);
		
	for (;;)
	{
		peeraddr_len = sizeof(peerinfo);
		memset(buf, 0, BUF_SIZE);
		nread = recvfrom(sfd, buf, BUF_SIZE, 0,
			       (struct sockaddr*) &peerinfo, &peeraddr_len);

		if (nread == -1)
		{
			fprintf(stderr, "server: could not read from peer bitch: %s\n", gai_strerror(nread));
			exit(EXIT_FAILURE);
		}

		getnameinfo((struct sockaddr*) &peerinfo, peeraddr_len,
			       host, NI_MAXHOST,
				service, NI_MAXSERV, 0);

		fprintf(stdout, "server: Received %s bytes from %s:%s bitch.\n",
				buf, host, service);

		if (sendto(sfd, buf, BUF_SIZE, 0, (struct sockaddr*) &peerinfo, peeraddr_len) < -1)
		{
			fprintf(stderr, "server: failed to send to client: %s:%s\n", host, service);
			exit(EXIT_FAILURE);
		}

	}		
	return 0;
}
