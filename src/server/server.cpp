#include "server.hpp"

#include <fcntl.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <csignal>
#include <cstdlib>
#include <cstring>
#include <iostream>

#include "game.hpp"
#include "parser.hpp"

struct client {
	std::string host;
	std::string port;
};

struct parameters {
	std::string plid;
	int time = -1;
	std::string code;
	int nt = 0;
};

volatile std::sig_atomic_t keepRunning = true;

void sigintHandler(int signal) {
	if (signal == SIGINT) keepRunning = false;
}

void sendUdpReply(int udp_fd, const std::string &message,
									const Client &client) {
	struct addrinfo hints, *res;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;

	if (getaddrinfo(client.host.c_str(), client.port.c_str(), &hints, &res) !=
			0) {
		perror("getaddrinfo");
		return;
	}

	sendto(udp_fd, message.c_str(), message.size(), 0, res->ai_addr,
				 res->ai_addrlen);
	freeaddrinfo(res);
}

int runServer(std::string port_number, bool verbose) {
	if (std::signal(SIGINT, sigintHandler) == SIG_ERR)
		std::cerr << "Failed changing how SIGINT is handled.\n";

	int udp_fd = socketInnit(port_number, UDP);
	int tcp_fd = socketInnit(port_number, TCP);

	if (verbose)
		std::cout << "The server is running on port " << port_number << ".\n";

	std::string udp_buf, tcp_buf;

	fd_set fds, fds_loop;
	struct timeval timeout;
	FD_ZERO(&fds);
	FD_SET(udp_fd, &fds);
	FD_SET(tcp_fd, &fds);

	Client client;
	Parameters parameters;

	int n;
	Command cmd;

	std::unordered_map<std::string, Player> players;

	while (keepRunning) {
		fds_loop = fds;
		memset(&timeout, 0, sizeof(struct timeval));
		timeout.tv_sec = 10;

		client.host = std::string(NI_MAXHOST, '\0');
		client.port = std::string(NI_MAXSERV, '\0');

		parameters.plid = std::string(PLID_LEN, '\0');
		parameters.time = -1;
		parameters.code = std::string(7, '\0');
		parameters.nt = 0;

		switch (select(FD_SETSIZE, &fds_loop, NULL, NULL, &timeout)) {
			case 0:
				std::cout << "Server is still running." << std::endl;

				break;
			case -1:
				if (errno != EINTR) errorErrno("select");
				break;
			default:
				if (FD_ISSET(udp_fd, &fds_loop)) {
					udp_buf.replace(0, UDP_SIZE, UDP_SIZE, '\0');
					n = readUdp(udp_fd, udp_buf.data(), client.host.data(),
											client.port.data());
					cmd = getCmd(udp_buf, n);
					if (cmd == CMD_ERR) {
						// sendUdpReply(udp_fd, "ERR", client); TODO: Send error

						if (verbose == true)
							printVerbose(false, parameters.plid, cmd, client);
						break;
					}
					std::string reply = parseAndRun(cmd, udp_buf, n, client, &parameters,
																					verbose, players);
					sendUdpReply(udp_fd, reply, client);
				}
				if (FD_ISSET(tcp_fd, &fds_loop)) {
					accept(tcp_fd, NULL, NULL);
					std::cout << "TCP client tried to connect\n";
				}
		}
	}

	close(udp_fd);
	close(tcp_fd);

	return 0;
}

int socketInnit(std::string port_number, bool protocol) {
	const char *tmp = port_number.c_str();
	struct addrinfo hints, *res;
	const int opt = 1;
	int fd, errcode;
	std::string s = protocol ? "UDP" : "TCP";
	memset(&hints, 0, sizeof(hints));
	if (protocol == UDP)
		hints.ai_socktype = SOCK_DGRAM;
	else
		hints.ai_socktype = SOCK_STREAM;
	hints.ai_family = AF_INET;
	hints.ai_flags = AI_PASSIVE;
	if ((errcode = getaddrinfo(NULL, tmp, &hints, &res)) != 0) errorGai(errcode);
	if ((fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1)
		errorErrno("socket");
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
		errorErrno("setsockopt");
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) == -1)
		errorErrno("setsockopt");
	if (bind(fd, res->ai_addr, res->ai_addrlen) == -1) errorErrno("bind");
	freeaddrinfo(res);
	if (protocol == TCP) {
		int flags;
		if (listen(fd, MAX_CONNECTIONS) == -1) errorErrno("listen");
		if ((flags = fcntl(fd, F_GETFL, 0)) == -1) errorErrno("fcntl");
		if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) errorErrno("fcntl");
	}
	std::cout << s << " socket set.\n";
	return fd;
}

int readUdp(int fd, char *buf, char *host, char *port) {
	struct sockaddr addr;
	socklen_t addrlen = sizeof(addr);
	ssize_t n = recvfrom(fd, buf, UDP_SIZE, 0, &addr, &addrlen);
	if (n == -1) errorErrno("recvfrom");
	if (getnameinfo(&addr, addrlen, host, NI_MAXHOST, port, NI_MAXSERV, 0) != 0)
		errorErrno("getnameinfo");
	return (int)n;
}

std::string parseAndRun(Command cmd, std::string buf, int len, Client client,
												Parameters *params, bool verbose,
												std::unordered_map<std::string, Player> &players) {
	bool parsing;
	buf.erase(0, CMD_LEN);
	len -= CMD_LEN;
	std::string prefix = "";
	std::string msg = "";

	switch (cmd) {
		case CMD_SNG:
			prefix = "RSG";

			parsing = parseSng(buf, len, params->plid, &params->time);
			if (parsing == false) {
				msg = "ERR";
			} else {
				msg = run_rsg(params, players);
			}
			break;
		case CMD_TRY:	 // TODO: Implement
			prefix = "RTR";

			parsing = parseTry(buf, len, params->plid, params->code, &params->nt);
			if (parsing == false) {
				msg = "ERR";
			} else {
				run_try(params, players);
			}
			break;
		case CMD_QUT:
			prefix = "RQT";

			parsing = parseQut(buf, len, params->plid);
			if (parsing == false) {
				msg = "ERR";
			} else {
				// status = run_qut()
				// reply(status)
			}
			break;
		case CMD_DBG:
			prefix = "RDB";
			parsing = parseDbg(buf, len, params->plid, &params->time, params->code);
			if (parsing == false) {
				msg = "ERR";
			} else {
				// status = run_dbg()
				// reply(status)
			}
			break;
		case CMD_STR:
		case CMD_SSB:
		case CMD_ERR:
			break;
		default:
			break;
	}
	if (verbose == true) printVerbose(parsing, params->plid, cmd, client);

	return prefix + " " + msg + "\n";
}

std::string run_rsg(Parameters *params,
										std::unordered_map<std::string, Player> &players) {
	std::string plid = params->plid;
	std::string msg = "";

	if (auto it = players.find(plid); it != players.end()) {
		// Player found
		if (it->second.getGame() == false) {
			it->second.startGame(params->time);
			msg = "OK";
			std::cout << "Started new game\n";
		} else {
			msg = "NOK";
			std::cout << "Game in progress\n";
		}
	} else {
		std::cout << "No player found\n";
		Player player(plid);
		player.startGame(params->time);
		players.insert({plid, player});
		msg = "OK";
		std::cout << "Started new game\n";
	}

	return msg;
}

std::string run_qut(
		Parameters *params,
		std::unordered_map<std::string, Player> &players) {	 // TODO: Reveal code
	std::string plid = params->plid;
	std::string msg = "";

	if (auto it = players.find(plid); it != players.end()) {
		// Player found
		if (it->second.getGame() == true) {
			it->second.endGame();
			msg = "OK";
			std::cout << "Ended game\n";
		} else {
			msg = "NOK";
			std::cout << "No game in progress\n";
		}
	} else {
		msg = "NOK";
		std::cout << "No player found\n";
	}

	return msg;
}

void run_try(Parameters *params,
						 std::unordered_map<std::string, Player> &players) {
	std::string plid = params->plid;
	if (auto it = players.find(plid); it != players.end()) {
		if (it->second.getGame() == true) {
			// GameUtils::checkGuess(); TODO: check guess
			// std::cout << "--------------------------\nCODE:\n"
			// 					<< params->code << "\n\n";
		} else {
			// std::cout << "This player has no game in progress\n";
		}
	} else {
		// std::cout << "No player found\n";
	}
}

// FIXME: corrigir cmd valid mas plid invalid
void printVerbose(bool parsing, std::string plid, Command cmd, Client client) {
	std::cout << "Received request:\n";
	if (parsing == false) {
		if (cmd == CMD_ERR) {
			std::cout << "\tError parsing the request;\n";
		} else {
			std::cout << "\tCommand - " << getCmdName(cmd) << ";\n";
			if (plid.at(0) != 0)
				std::cout << "\tPLID - " << plid << ";\n";
			else
				std::cout << "\tPLID - Error parsing the PLID;\n";
		}
	} else {
		std::cout << "\tCommand - " << getCmdName(cmd) << ";\n"
							<< "\tPLID - " << plid << ";\n";
	}
	std::cout << "\tHost - " << client.host << ";\n"
						<< "\tPort - " << client.port << ".\n";
}

void errorErrno(const char *func) {
	perror(func);
	exit(1);
}

void errorGai(int errcode) {
	std::cerr << "getaddrinfo: " << gai_strerror(errcode) << "\n";
	exit(1);
}
