#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

#define BUF_SIZE 500

int main(int argc, char** argv)
{
	if (argc < 4)
	{
		fprintf(stderr, "Usage: %s server_host server_port msg\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	
	int s, sfd;
	struct addrinfo *result, *rp;
	struct addrinfo hints;
	char buf[BUF_SIZE];

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = 0;

	s = getaddrinfo(argv[1], argv[2],
			&hints, &result);
	if (s == -1)
	{
		fprintf(stderr, "Failed to fetch address of host and serv: %s:%s: %s\n",
				argv[1], argv[2], gai_strerror(s));
		exit(EXIT_FAILURE);
	}

	for (rp = result; rp != NULL; rp = rp->ai_next)
	{
		sfd = socket(rp->ai_family, rp->ai_socktype,
				rp->ai_protocol);
		if (sfd == -1)
			continue;

		if (connect(sfd, rp->ai_addr,
					rp->ai_addrlen) == 0)
			break; /* success */
	}

	freeaddrinfo(result);

	if (rp == NULL)
	{
		fprintf(stderr, "Failed to connect.\n");
		exit(EXIT_FAILURE);
	}

	for (int j=3; j < argc; ++j)
	{
		if (write(sfd, argv[j], strlen(argv[j])) != strlen(argv[j]))
		{
			fprintf(stderr, "Failed to send all %zd bytes.\n", strlen(argv[j]));
			exit(EXIT_FAILURE);
		}

		if (s = read(sfd, buf, BUF_SIZE) > BUF_SIZE)
		{
			fprintf(stderr, "More bytes received than buffer size: %d vs %d.\n",
					s, BUF_SIZE);
			exit(EXIT_FAILURE);
		}

		char host[NI_MAXHOST], serv[NI_MAXSERV];
		getnameinfo(rp->ai_addr, rp->ai_addrlen,
				host, NI_MAXHOST,
				serv, NI_MAXSERV, 0);
		fprintf(stdout, "client: received %s from %s:%s\n", buf, host, serv);
	}

	return 0;
}



