#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define PORT 3500
#define MSG_SEP "\n"
#define BUFFER_SIZE 256
#define BACKLOG 32

int main(int argc, char* argv[])
{
	int fd_serveur = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in adresse_serveur;
	adresse_serveur.sin_family = AF_INET;
	adresse_serveur.sin_port = htons(PORT);
	adresse_serveur.sin_addr.s_addr = htonl(INADDR_ANY);


	// TODO use setsockopt
	int yes = 1;
    	if (setsockopt(fd_serveur, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int))
            == -1)
        	return -109;

	if (bind(fd_serveur, (struct sockaddr*)&adresse_serveur, sizeof(sockaddr_in)) == -1)
		return -101;

	if (listen(fd_serveur, BACKLOG) == -1)
		return -102;

	for (;;) // exit: CTRL+C
	{
		// useless?
		// struct sockaddr_in adresse_client;
		// socklen_t taille_adresse_client = sizeof(adresse_client.sin_addr);

		int fd_client = accept(fd_serveur, NULL, NULL);

		for (;;)
		{
			// TODO exit if "Bye"
			std::string message_client;
			char buffer[BUFFER_SIZE];

			for (;;)
			{
				int octets_lus = read(fd_client, buffer, 1);

				// TODO bug if user inputs CTRL+C
				// should I check for isalpha?

				if (octets_lus == 0) // connection closed
					break;

				if (octets_lus == -1) // internal error
					return -103;

				char c = buffer[0];
				if (c == '\n')
					break;

				message_client += c;
			}

			std::cout << message_client << std::endl;
			if (message_client == "bye")
				break;

			std::string message_serveur;
			std::cout << "> ";
			getline(std::cin, message_serveur);
			if (message_serveur.back() != '\n')
				message_serveur	+= '\n';

			write(fd_client, message_serveur.c_str(), message_serveur.length());
		}

		close (fd_client);
	}

	close(fd_serveur);

	return 0;
}

