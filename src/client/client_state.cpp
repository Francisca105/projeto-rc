#include "client_state.hpp"

#include <arpa/inet.h>
#include <unistd.h>

ClientState::ClientState(int argc, char **argv) {
	int opt, nopt = 0;

	while ((opt = getopt(argc, argv, "n:p:")) != -1) {
		switch (opt) {
			case 'n':
				this->setIp(optarg);
				nopt++;
				break;
			case 'p':
				this->setPort(optarg);
				nopt++;
				break;
			default:
				std::cerr << "Usage: " << argv[0] << " [-n GSIP] [-p GSport]\n";
				exit(1);
		}
	}

	if (argc != nopt * 2 + 1) {
		std::cerr << "Usage: " << argv[0] << " [-n GSIP] [-p GSport]\n";
		exit(1);
	}
}

// verify if IP is valid? if yes, use inet_pton
// use localhost if IP given as parameter is invalid?
void ClientState::setIp(std::string ip) {
	this->gs_ip = ip;
}

// verify if port is valid ? if yes, what conditions: valid number, > 1023, <
// 65536 ? use port 0 (allocated by the OS) if port is invalid ?
void ClientState::setPort(std::string port) {
	gs_port = port;
}

void ClientState::setPlid(std::string id) {
	plid = id;
}

std::string ClientState::getPlid() {
	return plid;
}

void initState(int argc, char **argv, ClientState &state) {
	int opt, nopt = 0;

	while ((opt = getopt(argc, argv, "n:p:")) != -1) {
		switch (opt) {
			case 'n':
				state.setIp(optarg);
				nopt++;
				break;
			case 'p':
				state.setPort(optarg);
				nopt++;
				break;
			default:
				std::cerr << "Usage: " << argv[0] << " [-n GSIP] [-p GSport]\n";
				exit(1);
		}
	}

	if (argc != nopt * 2 + 1) {
		std::cerr << "Usage: " << argv[0] << " [-n GSIP] [-p GSport]\n";
		exit(1);
	}
}
