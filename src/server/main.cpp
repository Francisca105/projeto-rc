#include "main.hpp"

#include <signal.h>

#include <cstring>
#include <iostream>

#include "config.hpp"
#include "server.hpp"

volatile sig_atomic_t KeepRunning = true;

int main(int argc, char **argv) {
	Config config;
	initConfig(&config, argc, argv);

	struct sigaction act;
	std::memset(&act, 0, sizeof(act));
	act.sa_handler = sigintHandler;
	if (sigaction(SIGINT, &act, NULL) == -1) {
		std::cerr
				<< "[LOG] Error changing SIGINT"
				<< std::endl;
		std::perror("[DEBUG] sigaction");
	}

	std::memset(&act, 0, sizeof(act));
	act.sa_handler = SIG_IGN;
	if (sigaction(SIGPIPE, &act, NULL) == -1) {
		std::cerr
				<< "[LOG] Error changing SIGPIPE"	<< std::endl;
		std::perror("[DEBUG] sigaction");
	}

	fd_set set, loop;
	FD_ZERO(&set);
	FD_SET(config.udp_fd, &set);
	FD_SET(config.tcp_fd, &set);
	struct timeval timeout;
	int ready;

	while (KeepRunning) {
		loop = set;
		std::memset(&timeout, 0, sizeof(timeout));
		timeout.tv_sec = SERVER_TIMEOUT;

		ready = select(FD_SETSIZE, &loop, NULL, NULL, &timeout);
		switch (ready) {
			case 0:
				std::cout << "Server is running" << std::endl;
				break;
			case -1:
				if (errno != EINTR) std::perror("[DEBUG] select");	// DEBUG
				break;
			default:
				if (FD_ISSET(config.udp_fd, &loop)) {
					handleUdp(config);
				}
				if (FD_ISSET(config.tcp_fd, &loop)) {
					// handleTcp();
				}
				break;
		}
	}

	std::cout << "Closing down the server..." << std::endl;

	return 0;
}

void sigintHandler(int signal) {
	(void)(signal);
	if (!KeepRunning) exit(EXIT_FAILURE);
	KeepRunning = false;
}
