#include "client.hpp"

#include <signal.h>

#include <cstring>
#include <iostream>

#include "client_game.hpp"
#include "client_parser.hpp"
#include "client_state.hpp"
#include "support.hpp"

int main(int argc, char **argv) {
	ClientState state;
	initClientState(&state, argc, argv);
	ClientArgs client_args;
	ServerArgs server_args;

	struct sigaction act;
	std::memset(&act, 0, sizeof(act));
	act.sa_handler = sigintHandler;
	if (sigaction(SIGINT, &act, NULL) == -1) {
		std::cerr << "An error has occurred while changing how SIGINT is handled."
							<< std::endl;	 // DEBUG
		std::perror("signal");	 // DEBUG
	}

	std::memset(&act, 0, sizeof(act));
	act.sa_handler = SIG_IGN;
	if (sigaction(SIGPIPE, &act, NULL) == -1) {
		std::cerr << "An error has occurred while changing how SIGPIPE is handled."
							<< std::endl;	 // DEBUG
		std::perror("signal");	 // DEBUG
	}

	std::string line;
	std::cout << "Insert command below:" << std::endl;
	while (KeepRunning) {
		std::cout << "> ";
		std::getline(std::cin, line);
		if (std::cin.fail() or std::cin.bad() or std::cin.eof()) break;
		if (isOnlyWhiteSpace(line)) continue;
		if (!handleCmd(line, &client_args, &server_args, &state)) break;	// TODO
		std::cout << "Insert command below:" << std::endl;
	}

	// TODO if a game is on, send QUT to the GS
	if (state.playing) runQuit(&server_args, &state);
	std::cout << "Closing down the application..." << std::endl;

	return EXIT_SUCCESS;
}

bool handleCmd(std::string &buf, ClientArgs *client_args,
							 ServerArgs *server_args, ClientState *state) {
	Command cmd = getCmd(buf);
	if (cmd == CMD_INV) {
		std::cerr << "Unknown command" << std::endl;
		return true;
	}
	if (parseCmd(buf, client_args, cmd)) {
		std::cerr << "Invalid syntax/parameters for the " << buf << " command"
							<< std::endl;
		return true;
	}
	if (!runCmd(cmd, *client_args, server_args, state)) return false;
	return true;
}

void sigintHandler(int signal) {
	(void)(signal);
	if (!KeepRunning) exit(EXIT_FAILURE);
	KeepRunning = false;
}

bool isOnlyWhiteSpace(std::string buf) {
	std::size_t len = buf.size();
	for (uint i = 0; i < len; i++)
		if (!std::isspace(buf.at(i))) return false;
	return true;
}
