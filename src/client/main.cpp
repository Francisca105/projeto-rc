#include "main.hpp"

#include <cstring>
#include <iostream>

#include "client.hpp"
#include "parser.hpp"
#include "state.hpp"
#include "support.hpp"

int main(int argc, char **argv) {
	Config config;
	initConfig(&config, argc, argv);
	State state;
	ClArgs cl_args;
	SvArgs sv_args;

	struct sigaction act;
	std::memset(&act, 0, sizeof(act));
	act.sa_handler = sigintHandler;
	if (sigaction(SIGINT, &act, NULL) == -1) {
		std::cerr << "An error has occurred while changing how SIGINT is handled"
							<< std::endl;
		std::perror("[DEBUG] sigaction");	 // DEBUG
	}

	std::memset(&act, 0, sizeof(act));
	act.sa_handler = SIG_IGN;
	if (sigaction(SIGPIPE, &act, NULL) == -1) {
		std::cerr << "An error has occurred while changing how SIGPIPE is handled"
							<< std::endl;
		std::perror("[DEBUG] sigaction");	 // DEBUG
	}

	std::string line;
	std::cout << "Insert command below:" << std::endl;
	while (KeepRunning) {
		std::cout << "> ";
		std::getline(std::cin, line);
		if (std::cin.fail() or std::cin.bad() or std::cin.eof()) break;
		if (isOnlyWhiteSpace(line)) continue;
		if (!handleCmd(line, cl_args, sv_args, config, &state)) {
			state.playing = false;	// fatal error - don't send message to server
			std::cerr << "A fatal error has occurred" << std::endl;
			break;
		}
		if (KeepRunning) std::cout << "Insert command below:" << std::endl;
	}

	if (state.playing) runQuit(sv_args, config, &state);

	std::cout << "\nClosing down the application...";

	return EXIT_SUCCESS;
}

bool handleCmd(std::string &buf, ClArgs cl_args, SvArgs sv_args, Config config,
							 State *state) {
	Cmd cmd = getCmd(buf);
	if (cmd == Invalid) {
		std::cerr << "Unknown command" << std::endl;
		return true;
	}
	if (parseCmd(buf, &cl_args, cmd)) {
		std::cerr << "Invalid syntax/parameters for the " << buf << " command"
							<< std::endl;
		return true;
	}
	return runCmd(cmd, cl_args, sv_args, config, state);
}

void sigintHandler(int signal) {
	(void)(signal);
	if (!KeepRunning) exit(EXIT_FAILURE);
	KeepRunning = false;
}

bool isOnlyWhiteSpace(std::string buf) {
	size_t len = buf.size();
	for (size_t i = 0; i < len; i++)
		if (!std::isspace(buf.at(i))) return false;
	return true;
}
