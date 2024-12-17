#include "client_state.hpp"

#include <unistd.h>

#include <iostream>

#include "client_protocol.hpp"

void initClientState(ClientState *state, int argc, char **argv) {
	int opt, nopt = 0;
	while ((opt = getopt(argc, argv, "n:p:")) != -1) {
		switch (opt) {
			case 'n':
				if (!validateIp(optarg)) {
					std::cout << "GSIP is not a valid IP address." << std::endl;
					std::cout << "Closing down the application..." << std::endl;
					exit(EXIT_SUCCESS);
				}
				state->ip = optarg;
				nopt++;
				break;
			case 'p':
				if (!validatePort(optarg)) {
					std::cout << "GSPort is not a valid port number." << std::endl;
					std::cout << "Closing down the application..." << std::endl;
					exit(EXIT_SUCCESS);
				}
				state->port = optarg;
				nopt++;
				break;
			default:
				std::cout << "Usage: " << argv[0] << " [-n GSIP] [-p GSport]"
									<< std::endl;
				exit(EXIT_SUCCESS);
		}
	}

	if (argc != 2 * nopt + 1) {
		std::cout << "Usage: " << argv[0] << " [-n GSIP] [-p GSport]" << std::endl;
		exit(EXIT_SUCCESS);
	}

	setUdpSocket(state->ip, state->port, &state->udp_fd, &state->server_addr);
}
