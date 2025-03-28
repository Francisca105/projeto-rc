#include "server.hpp"

#include <arpa/inet.h>
#include <time.h>
#include <unistd.h>

#include <filesystem>
#include <iostream>

#include "config.hpp"
#include "game.hpp"
#include "protocol.hpp"

void handleUdp(Config config) {
	Address addr;
	std::string request(MAX_REQUEST_LEN, '\0');

	if (!receiveUdp(request, config.udp_fd, &addr)) return;

	Cmd cmd = getCmd(request);
	if (cmd == Invalid) {
		if (config.verbose) {
			std::cout << "Unknown command" << std::endl;
		}
		std::string reply = "ERR\n";
		sendUdp(reply, config.udp_fd, addr);
		return;
	}

	Args args;
	if (!parseCmd(request, cmd, &args)) {
		std::cout << "Error parsing " << cmdAsStr(cmd) << std::endl;
		return;
	}

	runCmd(cmd, args, config, addr);
}

void runCmd(Cmd cmd, Args args, Config config, Address addr) {
	switch (cmd) {
		case Start:
			runStart(args, config, addr);
			if (config.verbose) printVerbose(args.plid, "SNG", addr);
			break;
		case Try:
			runTry(args, config, addr);
			if (config.verbose) printVerbose(args.plid, "TRY", addr);
			break;
		case Debug:
			runDebug(args, config, addr);
			if (config.verbose) printVerbose(args.plid, "DBG", addr);
			break;
		case Quit:
			runQuit(args, config, addr);
			if (config.verbose) printVerbose(args.plid, "QUT", addr);
			break;
		case Showtrials:
			runShowTrials(args, config);
			if (config.verbose) printVerbose(args.plid, "STR", addr);
			break;
		case Scoreboard:
			runScoreboard(config);
			if (config.verbose) printVerbose(args.plid, "SSB", addr);
			break;
		case Invalid:
		default:
			return;
	}
}

void runStart(Args args, Config config, Address addr) {
	std::string dummy;
	checkTimeout(args.plid, dummy);

	if (hasActiveGame(args.plid)) {
		if (config.verbose)
			printVerbose(args.plid, "Start game error (RSG NOK)", addr);
		std::string reply("RSG NOK\n");
		sendUdp(reply, config.udp_fd, addr);
		return;
	}

	std::string code(CODE_SIZE, '\0');
	if (!startGame(args.plid, args.time, code)) {
		if (config.verbose)
			printVerbose(args.plid, "Start game error (RSG ERR)", addr);
		std::string reply("RSG ERR\n");
		sendUdp(reply, config.udp_fd, addr);
		return;
	}

	if (config.verbose)
		printVerbose(args.plid, "Starting a new game (RSG OK)", addr);
	std::string reply("RSG OK\n");
	sendUdp(reply, config.udp_fd, addr);
}

void runTry(Args args, Config config, Address addr) {
	std::string code;
	if (checkTimeout(args.plid, code)) {
		if (config.verbose)
			printVerbose(args.plid, "Timeout - game closed (RTR ETM)", addr);
		std::string reply(sizeof("RTR ETM C C C C\n") - 1, '\0');
		sprintf(reply.data(), "RTR ETM %s\n", code.c_str());
		sendUdp(reply, config.udp_fd, addr);
		return;
	}

	if (hasActiveGame(args.plid)) {
		std::string reply;
		std::string tmp = getKey(args.plid), key(sizeof("C C C C") - 1, ' ');
		sprintf(key.data(), "%c %c %c %c", tmp[0], tmp[1], tmp[2], tmp[3]);
		int nB, nW;
		args.code.erase(1, 1).erase(2, 1).erase(3, 1).erase(4, 1);
		switch (checkTry(args.plid, args.code, args.nT)) {
			case OK:
				getHints(args.code, key, &nB, &nW);
				if (nB == CODE_SIZE) {
					if (config.verbose)
						printVerbose(args.plid, "Win - game closed (RTR OK)", addr);
					reply.resize(sizeof("RTR OK t 4 0\n") - 1, '\0');
					sprintf(reply.data(), "RTR OK %d 4 0\n", args.nT);
					registerTry(args.plid, args.code, nB, nW);
					winGame(args.plid, args.code, (char)(args.nT + '0'));
				} else {
					if (config.verbose)
						printVerbose(args.plid, "Valid trial (RTR OK)", addr);
					reply.resize(sizeof("RTR OK t b w\n") - 1, '\0');
					sprintf(reply.data(), "RTR OK %d %d %d\n", args.nT, nB, nW);
					registerTry(args.plid, args.code, nB, nW);
				}
				sendUdp(reply, config.udp_fd, addr);
				return;
			case Ok:
				if (config.verbose)
					printVerbose(args.plid, "Last trial repeat (RTR OK)", addr);
				reply.resize(sizeof("RTR OK t b w\n") - 1, '\0');
				sprintf(reply.data(), "RTR OK %d %d %d\n", args.nT, nB, nW);
				return;
			case DUP:
				if (config.verbose)
					printVerbose(args.plid, "Invalid trial (RTR DUP)", addr);
				reply = "RTR DUP\n";
				sendUdp(reply, config.udp_fd, addr);
				return;
			case INV:
				if (config.verbose)
					printVerbose(args.plid, "Invalid trial (RTR INV)", addr);
				reply = "RTR INV\n";
				sendUdp(reply, config.udp_fd, addr);
				return;
			case ENT:
				if (config.verbose)
					printVerbose(args.plid, "Fail - game lost (RTR ENT)", addr);
				reply.resize(sizeof("RTR ENT C C C C\n") - 1, '\0');
				sprintf(reply.data(), "RTR ENT %s\n", key.c_str());
				registerTry(args.plid, args.code, nB, nW);
				failGame(args.plid);
				sendUdp(reply, config.udp_fd, addr);
				return;
			default:
				break;
		};

	} else {
		if (config.verbose)
			printVerbose(args.plid, "Try guess error (RTR ERR)", addr);
		std::string reply("RTR NOK\n");
		sendUdp(reply, config.udp_fd, addr);
		return;
	}
}

void runDebug(Args args, Config config, Address addr) {
	std::string dummy;
	checkTimeout(args.plid, dummy);

	if (hasActiveGame(args.plid)) {
		if (config.verbose)
			printVerbose(args.plid, "Debug game error (RDB NOK)", addr);
		std::string reply("RDB NOK\n");
		sendUdp(reply, config.udp_fd, addr);
		return;
	}

	if (!startGame(args.plid, args.time,
								 args.code.erase(1, 1).erase(2, 1).erase(3, 1).erase(4, 1))) {
		if (config.verbose)
			printVerbose(args.plid, "Debug game error (RDB ERR)", addr);
		std::string reply("RDB ERR\n");
		sendUdp(reply, config.udp_fd, addr);
		return;
	}

	if (config.verbose)
		printVerbose(args.plid, "Starting a new game in debug mode (RDB OK)", addr);
	std::string reply("RDB OK\n");
	sendUdp(reply, config.udp_fd, addr);
}

void runQuit(Args args, Config config, Address addr) {
	std::string dummy;
	checkTimeout(args.plid, dummy);

	if (hasActiveGame(args.plid)) {
		if (config.verbose) printVerbose(args.plid, "Quitting game (QUT OK)", addr);
		std::string key(sizeof("C C C C") - 1, '\0');
		quitGame(args.plid, key);
		std::string reply(sizeof("RQT OK C C C C\n") - 1, '\0');
		sprintf(reply.data(), "RQT OK %s\n", key.c_str());
		sendUdp(reply, config.udp_fd, addr);
		return;
	} else {
		if (config.verbose)
			printVerbose(args.plid, "Quitting game error (QUT NOK)", addr);
		std::string reply("RQT NOK\n");
		sendUdp(reply, config.udp_fd, addr);
	}
}

void printVerbose(std::string plid, std::string request, Address addr) {
	char ip[NI_MAXHOST], port[NI_MAXSERV];
	int errcode = getnameinfo((struct sockaddr *)&(addr.addr), addr.addrlen, ip,
														sizeof(ip), port, sizeof(port), 0);
	if (errcode != 0) {
		std::cerr << "Error getting player info" << std::endl;
	}
	std::cout << plid << " - " << request << std::endl
						<< "sent by [" << ip << ":" << port << "]" << std::endl;
}

void printVerbose(std::string plid, std::string request, int fd) {
	char ipstr[INET6_ADDRSTRLEN];
	int port;
	sockaddr addr;
	socklen_t len = sizeof(addr);
	int errcode = getpeername(fd, &addr, &len);
	if (errcode != 0) {
		std::cerr << "Error getting player info" << std::endl;
	}
	struct sockaddr_in *s = (struct sockaddr_in *)&addr;
	port = ntohs(s->sin_port);
	inet_ntop(AF_INET, &s->sin_addr, ipstr, sizeof(ipstr));
	std::cout << plid << " - " << request << std::endl
						<< "sent by [" << ipstr << ":" << port << "]" << std::endl;
}

void printVerbose(std::string request, int fd) {
	char ipstr[INET6_ADDRSTRLEN];
	int port;
	sockaddr addr;
	socklen_t len = sizeof(addr);
	int errcode = getpeername(fd, &addr, &len);
	if (errcode != 0) {
		std::cerr << "Error getting player info" << std::endl;
	}
	struct sockaddr_in *s = (struct sockaddr_in *)&addr;
	port = ntohs(s->sin_port);
	inet_ntop(AF_INET, &s->sin_addr, ipstr, sizeof(ipstr));
	std::cout << request << std::endl
						<< "sent by [" << ipstr << ":" << port << "]" << std::endl;
}

void handleTcp(Config config) {
	Address addr;
	int fd = accept(config.tcp_fd, &addr.addr, &addr.addrlen);
	std::string request;
	if (!receiveTcp(request, fd)) {
		return;
	}

	Cmd cmd = getCmd(request);
	if (cmd == Invalid) {
		std::cout << "Unknown command" << std::endl;
		std::string reply = "ERR\n";
		sendTcp(reply, fd);
		return;
	}

	Args args;
	if (!parseCmd(request, cmd, &args)) {
		std::cout << "Error parsing " << cmdAsStr(cmd) << std::endl;
		return;
	}

	config.tcp_fd = fd;

	runCmd(cmd, args, config, addr);
	close(fd);
}

void runShowTrials(Args args, Config config) {
	std::string dummy;
	checkTimeout(args.plid, dummy);

	if (hasActiveGame(args.plid)) {
		if (config.verbose)
			printVerbose(args.plid, "Sending active trials file (RST ACT)",
									 config.tcp_fd);
		sendActiveGame(args.plid, config.tcp_fd);
	} else if (hasFinishedGame(args.plid)) {
		if (config.verbose)
			printVerbose(args.plid, "Sending finished trials file (RST FIN)",
									 config.tcp_fd);
		sendLastGame(args.plid, config.tcp_fd);
	} else {
		if (config.verbose)
			printVerbose(args.plid, "ShowTrials error (RST NOK)", config.tcp_fd);
		std::string reply("RST NOK\n");
		sendTcp(reply, config.tcp_fd);
	}
}

void runScoreboard(Config config) {
	std::filesystem::path p = std::filesystem::current_path();
	if (std::filesystem::is_empty(p / "server/scores/")) {
		if (config.verbose)
			printVerbose("Scoreboard empty (RSS EMPTY)", config.tcp_fd);
		std::string reply("RSS EMPTY\n");
		sendTcp(reply, config.tcp_fd);
	} else {
		if (config.verbose)
			printVerbose("Sending scoreboard (RSS OK)", config.tcp_fd);
		sendScoreboard(config.tcp_fd, &config.scoreboard);
	}
}
