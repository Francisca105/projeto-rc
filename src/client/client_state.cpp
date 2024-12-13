#include "client_state.hpp"

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>

#include <cstring>
#include <iostream>

#include "../common/transport.hpp"

int initClientState(ClientState *state, int argc, char **argv) {
	int opt, nopt = 0;
	while ((opt = getopt(argc, argv, "n:p:")) != -1) {
		switch (opt) {
			case 'n':
				setIp(optarg, state->ip);
				nopt++;
				break;
			case 'p':
				setPort(optarg, state->port);
				nopt++;
				break;
			default:
				std::cerr << "Usage: " << argv[0] << " [-n GSIP] [-p GSport]\n";
				exit(1);
		}
	}

	if (argc != 2 * nopt + 1) {
		std::cerr << "Usage: " << argv[0] << " [-n GSIP] [-p GSport]\n";
		exit(1);
	}

	state->fd = setUdpSocket(state->ip, state->port, &state->addr, false);

	return 0;
}

void setIp(const char *buf, std::string &ip) {
	char tmp[sizeof(struct in_addr)];
	if (!inet_pton(AF_INET, buf, tmp)) {
		std::cerr << "GSIP not a valid IP address.\n"
							<< "Error - inet_pton\n";
		exit(1);
	}
	ip = buf;
}

void setPort(const char *buf, std::string &port) {
	char *end_ptr{};
	errno = 0;
	long port_number = std::strtol(buf, &end_ptr, 10);
	if (port_number == 0) {
		std::cerr << "GSport not a valid number.\n";
		exit(1);
	} else if (errno == ERANGE or port_number < 0 or
						 port_number > (1 << 16) - 1) {
		std::cerr << "GSport not a valid port number.\n";
		exit(1);
	}
	port = buf;
}
