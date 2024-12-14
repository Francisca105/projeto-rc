#include "client.hpp"

#include <signal.h>

#include <cstring>
#include <iostream>

#include "client_game.hpp"
#include "client_parser.hpp"
#include "client_state.hpp"

volatile sig_atomic_t keep_running = true;

int main(int argc, char **argv) {
	setSignal(SIGINT, sigintHandler);

	ClientState state;
	initClientState(&state, argc, argv);
	ClientArgs client_args;
	ServerArgs server_args;

	std::string line;
	std::cout << "Insert command below:\n";
	while (keep_running) {
		std::cout << "> ";
		std::getline(std::cin, line);
		if (std::cin.fail() or std::cin.bad() or std::cin.eof()) break;
		if (isOnlyWhiteSpace(line)) continue;
		handleCmd(line, &client_args, &server_args, &state);
		std::cout << "Insert command below:\n";
	}

	// send QUT to the GS
	std::cout << "\nClosing the player application ...";

	return 0;
}

void sigintHandler(int signal) {
	if (signal == SIGINT) keep_running = false;
}

void setSignal(int signal, void (*func)(int)) {
	struct sigaction act;
	std::memset(&act, 0, sizeof(act));
	act.sa_handler = func;
	if (sigaction(signal, &act, NULL) == -1) {
		std::cerr << "Could not change how SIGINT is handled.\n";
		std::perror("Error - sigaction");
	}
}

bool isOnlyWhiteSpace(std::string s) {
	std::size_t len = s.size();
	for (uint i = 0; i < len; i++)
		if (!std::isspace(s.at(i))) return false;
	return true;
}

void handleCmd(std::string &s, ClientArgs *client_args, ServerArgs *server_args,
							 ClientState *state) {
	Command cmd = getCmd(s);
	if (cmd == CMD_ERR) {
		std::cerr << "Unknown command.\n";
		return;
	}
	if (!parseCmd(s, client_args, cmd)) {
		std::cerr << "Error parsing " << s << "\n";
		return;
	}
	runCmd(cmd, *client_args, server_args, state);
}
