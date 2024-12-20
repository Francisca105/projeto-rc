#include "server.hpp"

#include <time.h>

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
		std::cout << "[LOG] Unknown command" << std::endl;
		std::string reply = "ERR\n";
		sendUdp(reply, config.udp_fd, addr);
		return;
	}

	Args args;
	if (!parseCmd(request, cmd, &args)) {
		std::cout << "[LOG] Error parsing " << cmdAsStr(cmd) << std::endl;
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
			if (config.verbose) printVerbose(args.plid, "DBG", addr);
			break;
		case Showtrials:
			// runShowTrials(args, config, addr);
			if (config.verbose) printVerbose(args.plid, "DBG", addr);
			break;
		case Scoreboard:
			// runScoreboard(args, config, addr);
			if (config.verbose) printVerbose(args.plid, "DBG", addr);
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
		std::cout << "[LOG] " << args.plid << " - Game already in progress" << std::endl;
		std::string reply("RSG NOK\n");
		sendUdp(reply, config.udp_fd, addr);
		return;
	}

	std::string code(CODE_SIZE, '\0');
	if (!startGame(args.plid, args.time, code)) {
		std::cout << "[LOG] " << args.plid << " - Error starting game" << std::endl;
		std::string reply("RSG ERR\n");
		sendUdp(reply, config.udp_fd, addr);
		return;
	}

	std::cout << "[LOG] " << args.plid << " - Started new game (max " << args.time << " s) (code " << code << ") (normal)" << std::endl;
	std::string reply("RSG OK\n");
	sendUdp(reply, config.udp_fd, addr);
}

void runTry(Args args, Config config, Address addr) {
	std::string code;
	if (checkTimeout(args.plid, code)) {
		// std::cout << "[LOG] " << args.plid << " - " << std::endl;
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
				aOk(args.code, key, &nB, &nW);
				if (nB == CODE_SIZE) {
					reply.resize(sizeof("RTR OK t 4 0\n") - 1, '\0');
					sprintf(reply.data(), "RTR OK %d 4 0\n", args.nT);
					std::cout<<reply;
					registerTry(args.plid, args.code, nB, nW);
					winGame(args.plid, args.code, (char)(args.nT + '0'));
				} else {
					reply.resize(sizeof("RTR OK t b w\n") - 1, '\0');
					sprintf(reply.data(), "RTR OK %d %d %d\n", args.nT, nB, nW);
					registerTry(args.plid, args.code, nB, nW);
				}
				sendUdp(reply, config.udp_fd, addr);
				return;
			case Ok:
				reply.resize(sizeof("RTR OK t b w\n") - 1, '\0');
				sprintf(reply.data(), "RTR OK %d %d %d\n", args.nT, nB, nW);
				return;
			case DUP:
				reply = "RTR DUP\n";
				sendUdp(reply, config.udp_fd, addr);
				return;
			case INV:
				reply = "RTR INV\n";
				sendUdp(reply, config.udp_fd, addr);
				return;
			case ENT:
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
		std::cout << "[LOG] " << args.plid << " - No active game" << std::endl;
		std::string reply("RTR NOK\n");
		sendUdp(reply, config.udp_fd, addr);
		return;
	}

}

void runDebug(Args args, Config config, Address addr) {
	std::string dummy;
	checkTimeout(args.plid, dummy);

	if (hasActiveGame(args.plid)) {
		std::cout << "[LOG] " << args.plid << " - Game already in progress" << std::endl;
		std::string reply("RDB NOK\n");
		sendUdp(reply, config.udp_fd, addr);
		return;
	}

	if (!startGame(args.plid, args.time, args.code.erase(1,1).erase(2,1).erase(3,1).erase(4,1))) {
		std::cout << "[LOG] " << args.plid << " - Error starting game" << std::endl;
		std::string reply("RDB ERR\n");
		sendUdp(reply, config.udp_fd, addr);
		return;
	}

	std::cout << "[LOG] " << args.plid << " - Started new game (max " << args.time << " s) (code " << args.code << ") (debug)" << std::endl;
	std::string reply("RDB OK\n");
	sendUdp(reply, config.udp_fd, addr);
}

void runQuit(Args args, Config config, Address addr) {
	std::string dummy;
	checkTimeout(args.plid, dummy);

	if (hasActiveGame(args.plid)) {
		// std::cout << "[LOG] " << args.plid << " - Game already in progress" << std::endl;
		std::string key(sizeof("C C C C") - 1, '\0');
		quitGame(args.plid, key);
		std::string reply(sizeof("RQT OK C C C C\n") - 1, '\0');
		sprintf(reply.data(), "RQT OK %s\n", key.c_str());
		sendUdp(reply, config.udp_fd, addr);
		return;
	} else {
		std::string reply("RQT NOK\n");
		sendUdp(reply, config.udp_fd, addr);
	}
}

void printVerbose(std::string plid, std::string request, Address addr) {
	char ip[NI_MAXHOST], port[NI_MAXSERV];
	int errcode = getnameinfo((struct sockaddr *)&(addr.addr), addr.addrlen, ip,
														sizeof(ip), port, sizeof(port), 0);
	if (errcode != 0) {
		std::cerr << "[DEBUG] Error getting player info" << std::endl;
	}
	std::cout << "Verbose:\n  PLID - " << plid << "\n  Request - " << request
						<< "\n  Player IP - " << ip << "\n  Player port - " << port
						<< std::endl;
}

void handleTcp(Config config) {
	Address addr;
	int fd = accept(config.tcp_fd, &addr.addr, &addr.addrlen);

	std::string request(sizeof("STR PPPPPP\n") - 1, '\0');
	if (!receiveTcp(request, fd)) return;

	Cmd cmd = getCmd(request);
	if (cmd == Invalid) {
		std::cout << "[LOG] Unknown command" << std::endl;
		std::string reply = "ERR\n";
		// sendTcp(reply, config.udp_fd, addr);
		return;
	}

	Args args;
	if (!parseCmd(request, cmd, &args)) {
		std::cout << "[LOG] Error parsing " << cmdAsStr(cmd) << std::endl;
		return;
	}

	config.tcp_fd = fd;

	runCmd(cmd, args, config, addr);
}

// void runShowTrials(Args args, Config config, Address addr) {
// 	std::string dummy;
// 	checkTimeout(args.plid, dummy);

// 	if (hasActiveGame(args.plid)) {
// 		sendActiveGame();
// 	} else if hasFinishedGame(args.plid) {
// 		sendLastGame();
// 	} else {
// 		std::string reply("RST NOK\n");
// 		sendTcp(reply, )
// 	}
// }
