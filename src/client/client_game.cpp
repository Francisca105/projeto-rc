#include "client_game.hpp"

#include <iostream>

#include "../common/transport.hpp"
#include "client_parser.hpp"

void runCmd(Command cmd, ClientArgs client_args, ServerArgs *server_args,
						ClientState *state) {
	if (client_args.time == 0 and state->fd == -1) return;
	switch (cmd) {
		case CMD_START:
			runStart(client_args, server_args, state);
			break;
		case CMD_TRY:
			// runTry(args, state);
			break;
		case CMD_QUIT:
			// runQuit(args, state);
			break;
		case CMD_EXIT:
			// runExit(args, state);
			break;
		case CMD_DEBUG:
			// runDebug(args, state);
			break;
		case CMD_SHOWTRIALS:
			// runShowTrials(args, state);
			break;
		case CMD_SCOREBOARD:
			// runScoreboard(args, state);
			break;
		case CMD_ERR:
		default:
			break;
	}
}

void runStart(ClientArgs client_args, ServerArgs *server_args,
							ClientState *state) {
	if (state->plid.size() > 0 and client_args.plid.compare(state->plid) != 0) {
		std::cerr << "There is a player already active.\n";
		return;
	}
	std::string packet(SNG_LEN, ' ');
	sprintf(packet.data(), "SNG %s %3d\n", client_args.plid.c_str(),
					client_args.time);
	std::string reply(MAX_UDP_REPLY, ' ');
	if (!sendUdpAndWait(state->fd, packet, reply, *state->addr, nullptr)) {
		std::cerr << "Game Server did not replied to the request.\n";
		return;
	}
	if (!parseRSG(reply, server_args)) {
		std::cerr << "Something wrong happened with the server.\n"
							<< "Closing the application ...";
		exit(1);
	}
	std::cout << reply;
	if (server_args->status == OK) {
		state->plid = client_args.plid;
		state->nT = 0;
	}
}

// void runTry(ClientArgs client_args, ServerArgs *server_args,
// 							ClientState *state) {
// 	if (!state->plid.size()) {
// 		std::cerr << "There is no player active in the application.\n";
// 		return;
// 	}
// 	std::string packet(TRY_LEN, ' ');
// 	sprintf(packet.data(), "TRY %s %s %d\n", client_args.plid.c_str(),
// 					client_args.code, state->nT);
// 	std::string reply(MAX_UDP_REPLY, ' ');
// 	if (!sendUdpAndWait(state->fd, packet, reply, *state->addr, nullptr)) {
// 		std::cerr << "Game Server did not replied to the request.\n";
// 		return;
// 	}
// 	if (!parseRTR(reply, server_args)) {
// 		std::cerr << "Something wrong happened with the server.\n"
// 							<< "Closing the application ...";
// 		exit(1);
// 	}
// 	std::cout << reply;
// }

// void runQuit(ClientArgs args, ClientState *state) {

// }

// void runExit(ClientArgs args, ClientState *state) {

// }

// void runDebug(ClientArgs args, ClientState *state) {

// }

// void runShowTrials(ClientArgs args, ClientState *state) {

// }

// void runScoreboard(ClientArgs args, ClientState *state) {

// }
