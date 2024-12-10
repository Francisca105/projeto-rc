#include "client.hpp"

#include <signal.h>

#include <cstring>
#include <iostream>

#include "client_game.hpp"
#include "client_state.hpp"

volatile sig_atomic_t keep_running = true;

void sigintHandler(int signum);

// use exceptions ?
// continue program if error is not fatal ?
int main(int argc, char **argv) {
	struct sigaction act;
	memset(&act, 0, sizeof(act));
	act.sa_handler = sigintHandler;
	if (sigaction(SIGINT, &act, NULL) == -1)
		std::cerr << "Failed changing how SIGINT is handled.\n";

	ClientState client_state(argc, argv);

	std::string line;
	Input input;
	while (keep_running) {
		std::getline(std::cin, line);
		if (std::cin.fail() or std::cin.eof()) break; // necessary ?
		Command cmd = getCmd(line);
		if (cmd != CMD_ERR)
			parseCmd(line, &input, cmd);
		else
			std::cout << "Command type not identified.\n";
	}

	// sends QUT to the server and closes client
	std::cout << "\nClosing client...\n";

	return 0;
}

void sigintHandler(int signum) {
	if (signum == SIGINT) keep_running = false;
}
