#include "config.hpp"

#include <netdb.h>
#include <unistd.h>

#include <cstdlib>
#include <cstring>
#include <iostream>

void initConfig(Config *config, int argc, char **argv) {
	int opt;
	double nopt = 0;
	while ((opt = getopt(argc, argv, "vp:")) != -1) {
		switch (opt) {
			case 'v':
				config->verbose = true;
				nopt += 0.5;
				break;
			case 'p':
				if (!validatePort(optarg)) {
					std::cout << "GSPort is not a valid port number" << std::endl;
					std::cout << "Closing down the server..." << std::endl;
					exit(EXIT_FAILURE);
				}
				config->port = optarg;
				nopt++;
				break;
			default:
				std::cout << "Usage: " << argv[0] << " [-p GSport] [-v]" << std::endl;
				exit(EXIT_FAILURE);
		}
	}

	if (argc > 2 * nopt + 1 or argc < 2 * nopt + 1) {
		std::cout << "Usage: " << argv[0] << " [-p GSport] [-v]" << std::endl;
		exit(EXIT_FAILURE);
	}

	setUdpSocket(config->port, &config->udp_fd);
	setTcpSocket(config->port, &config->tcp_fd);
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

void setUdpSocket(std::string port, int *fd) {
	if ((*fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		std::cerr << "Error creating UDP socket" << std::endl;
		std::perror("[DEBUG] socket");
		std::cout << "Closing down the server..." << std::endl;
		exit(EXIT_FAILURE);
	}

	struct addrinfo hints, *res;
	std::memset(&hints, 0, sizeof(hints));
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_family = AF_INET;
	hints.ai_flags = AI_PASSIVE;

	int errcode = getaddrinfo(NULL, port.c_str(), &hints, &res);
	if (errcode != 0) {
		std::cerr << "Error getting server address" << std::endl;
		std::cerr << "[DEBUG] getaddrinfo: " << gai_strerror(errcode) << std::endl;
		std::cout << "Closing down the server..." << std::endl;
		exit(EXIT_FAILURE);
	}

	if (bind(*fd, res->ai_addr, res->ai_addrlen) == -1) {
		std::cerr << "Error binding UDP socket" << std::endl;
		std::perror("[DEBUG] bind");
		std::cout << "Closing down the server..." << std::endl;
		exit(EXIT_FAILURE);
	}
}

void setTcpSocket(std::string port, int *fd) {
	if ((*fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		std::cerr << "Error creating TCP socket" << std::endl;
		std::perror("[DEBUG] socket");
		std::cout << "Closing down the server..." << std::endl;
		exit(EXIT_FAILURE);
	}

	const int enable = 1;
	if (setsockopt(*fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
		std::cerr << "setsockopt(SO_REUSEADDR) failed";

	if (setsockopt(*fd, SOL_SOCKET, SO_REUSEPORT, &enable, sizeof(int)) < 0)
		std::cerr << "setsockopt(SO_REUSEADDR) failed";

	struct addrinfo hints, *res;
	std::memset(&hints, 0, sizeof(hints));
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_family = AF_INET;
	hints.ai_flags = AI_PASSIVE;

	int errcode = getaddrinfo(NULL, port.c_str(), &hints, &res);
	if (errcode != 0) {
		std::cerr << "Error getting server address" << std::endl;
		std::cerr << "[DEBUG] getaddrinfo: " << gai_strerror(errcode) << std::endl;
		std::cout << "Closing down the server..." << std::endl;
		exit(EXIT_FAILURE);
	}

	if (bind(*fd, res->ai_addr, res->ai_addrlen) == -1) {
		std::cerr << "Error binding TCP socket" << std::endl;
		std::perror("[DEBUG] bind");
		std::cout << "Closing down the server..." << std::endl;
		exit(EXIT_FAILURE);
	}

	if (listen(*fd, MAX_TCP_CONNECTIONS) == -1) {
		std::cerr << "Error listening TCP socket" << std::endl;
		std::perror("[DEBUG] listen");
		std::cout << "Closing down the server..." << std::endl;
		exit(EXIT_FAILURE);
	}
}
