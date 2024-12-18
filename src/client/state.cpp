#include "state.hpp"

#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

#include <cstring>
#include <iostream>

void initConfig(Config *config, int argc, char **argv) {
	int opt, nopt = 0;
	while ((opt = getopt(argc, argv, "n:p:")) != -1) {
		switch (opt) {
			case 'n':
				config->ip = optarg;
				nopt++;
				break;
			case 'p':
				if (!validatePort(optarg)) {
					std::cout << "GSPort is not a valid port number" << std::endl;
					std::cout << "Closing down the application..." << std::endl;
					exit(EXIT_FAILURE);
				}
				config->port = optarg;
				nopt++;
				break;
			default:
				std::cout << "Usage: " << argv[0] << " [-n GSIP] [-p GSport]"
									<< std::endl;
				exit(EXIT_FAILURE);
		}
	}

	if (argc != 2 * nopt + 1) {
		std::cout << "Usage: " << argv[0] << " [-n GSIP] [-p GSport]" << std::endl;
		exit(EXIT_FAILURE);
	}

	setUdpSocket(config->ip, config->port, &config->udp_fd, &config->server_addr);
}

bool validateIp(const char *ip) {
	char tmp[sizeof(struct in_addr)];
	if (inet_pton(AF_INET, ip, tmp) == 0) return false;
	return true;
}

bool validatePort(const char *port) {
	errno = 0;
	char *end_ptr{};
	long port_number = std::strtol(port, &end_ptr, 10);
	if (port_number == 0) return false;
	if (errno == ERANGE or port_number < 0 or port_number > (1 << 16) - 1)
		return false;
	return true;
}

void setUdpSocket(std::string ip, std::string port, int *fd,
									struct addrinfo **addr) {
	if ((*fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		std::cerr << "Failed to create the UDP socket" << std::endl;
		std::perror("[DEBUG] socket");	// DEBUG
		std::cout << "Closing down the application..." << std::endl;
		exit(EXIT_FAILURE);
	}

	struct addrinfo hints;
	std::memset(&hints, 0, sizeof(hints));
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_family = AF_INET;

	int errcode = getaddrinfo(ip.c_str(), port.c_str(), &hints, addr);
	if (errcode != 0) {
		std::cerr << "Failed to get the Game Server address" << std::endl;
		std::cerr << "[DEBUG] getaddrinfo: " << gai_strerror(errcode)
							<< std::endl;	 // DEBUG
		std::cout << "Closing down the application..." << std::endl;
		close(*fd);
		exit(EXIT_FAILURE);
	}
}
