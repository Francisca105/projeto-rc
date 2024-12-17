#include "client_game.hpp"

#include <iostream>

#include "client_parser.hpp"
#include "client_protocol.hpp"

bool runCmd(Command cmd, ClientArgs client_args, ServerArgs *server_args,
						ClientState *state) {
	switch (cmd) {
		case CMD_START:
			if (!runStart(client_args, server_args, state)) return false;
			break;
		case CMD_TRY:
			if (!runTry(client_args, server_args, state)) return false;
			break;
		case CMD_QUIT:
			if (!runQuit(server_args, state)) return false;
			break;
		case CMD_EXIT:
			runQuit(server_args, state);
			return false;
		case CMD_DEBUG:
			if (!runDebug(client_args, server_args, state)) return false;
			break;
		case CMD_SHOWTRIALS:
			runShowTrials(server_args, state);
			break;
		case CMD_SCOREBOARD:
			// runScoreboard(args, state);
			break;
		case CMD_INV:
		default:
			break;
	}
	return true;
}

bool runStart(ClientArgs client_args, ServerArgs *server_args,
							ClientState *state) {
	if (state->playing and client_args.plid.compare(state->plid) != 0) {
		std::cout << "Another player already playing (" << state->plid << ")"
							<< std::endl;
		return true;
	}

	std::string request = createPacket(CMD_START, client_args, *state, true);
	std::string reply = createPacket(CMD_START, client_args, *state, false);

	Packet packet =
			sendUdpAndWait(state->udp_fd, request, reply, *state->server_addr);
	if (packet == NOT_SENT or packet == NOT_REPLIED)
		return true;
	else if (packet == INTERRUPT)
		return false;

	if (!parseRSG(reply, server_args)) {
		std::cerr << "There was a problem with the reply received from the Game "
								 "Server\nClosing down the application..."
							<< std::endl;	 // DEBUG
		close(state->udp_fd);
		exit(EXIT_SUCCESS);
	}

	Status status = server_args->status;
	if (status == NOK) {
		std::cout << "Game already in progress" << std::endl;
	} else if (status == OK) {
		std::cout << "New game started (max " << client_args.time << " seconds)"
							<< std::endl;
		state->playing = true;
		state->plid = client_args.plid;
		state->nT = 1;
	} else {
		std::cerr << "Message not recognized by the server" << std::endl;	 // DEBUG
	}

	return true;
}

bool runTry(ClientArgs client_args, ServerArgs *server_args,
						ClientState *state) {
	if (!state->playing) {
		std::cout << "No game active" << std::endl;
		return true;
	}

	std::string request = createPacket(CMD_TRY, client_args, *state, true);
	std::string reply = createPacket(CMD_TRY, client_args, *state, false);

	Packet packet =
			sendUdpAndWait(state->udp_fd, request, reply, *state->server_addr);
	if (packet == NOT_SENT or packet == NOT_REPLIED)
		return true;
	else if (packet == INTERRUPT)
		return false;

	if (!parseRTR(reply, server_args)) {
		std::cerr << "There was a problem with the reply received from the Game "
								 "Server\nClosing down the application..."
							<< std::endl;	 // DEBUG
		close(state->udp_fd);
		exit(EXIT_SUCCESS);
	}

	Status status = server_args->status;
	if (status == OK) {
		if (server_args->nB == 4) {
			std::cout << "Well done! You guessed the key in " << server_args->nT
								<< " trials" << std::endl;
		} else {
			std::cout << "nB = " << server_args->nB << ", nW = " << server_args->nW
								<< std::endl;
			state->nT += 1;
			// TODO check "same reply if nT is expected - 1 -> trials not increased"
		}
	} else if (status == DUP) {
		// DEBUG This would happen if there's a timeout and we don't read (i think)
		std::cout << "That was a repeat of a previous trial" << std::endl;
	} else if (status == INV) {
		// DEBUG this should not happen (i think, didn't think a lot about it)
		std::cerr << "Game server replied with INV" << std::endl;
	} else if (status == NOK) {
		// DEBUG this should not happen
		std::cerr << "Game Server replied with NOK" << std::endl;
	} else if (status == ENT) {
		std::cout << "Oops! You ran out of tries. The secret key was: "
							<< server_args->code << std::endl;
		state->playing = false;
		state->plid.clear();
	} else if (status == ETM) {
		std::cout << "Oops! You ran out of time. The secret key was: "
							<< server_args->code << std::endl;
		state->playing = false;
		state->plid.clear();
	} else {
		std::cerr << "Message not recognized by the server" << std::endl;	 // DEBUG
	}

	return true;
}

bool runQuit(ServerArgs *server_args, ClientState *state) {
	if (!state->playing) {
		std::cout << "No game active" << std::endl;
		return true;
	}

	std::string request = createPacket(CMD_QUIT, true);
	std::string reply = createPacket(CMD_QUIT, false);

	Packet packet =
			sendUdpAndWait(state->udp_fd, request, reply, *state->server_addr);
	if (packet == NOT_SENT or packet == NOT_REPLIED)
		return true;
	else if (packet == INTERRUPT)
		return false;

	if (!parseQUT(reply, server_args)) {
		std::cerr << "There was a problem with the reply received from the Game "
								 "Server\nClosing down the application..."
							<< std::endl;	 // DEBUG
		close(state->udp_fd);
		exit(EXIT_SUCCESS);
	}

	Status status = server_args->status;
	if (status == OK) {
		std::cout << "Game ended. The secret key was: " << server_args->code
							<< std::endl;
		state->playing = false;
		state->plid.clear();
	} else if (status == NOK) {
		std::cout << "There was not a game in progress" << std::endl;
		state->playing = false;
		state->plid.clear();
	} else {
		std::cerr << "Message not recognized by the server" << std::endl;	 // DEBUG
	}

	return true;
}

bool runDebug(ClientArgs client_args, ServerArgs *server_args,
							ClientState *state) {
	if (state->playing and client_args.plid.compare(state->plid) != 0) {
		std::cout << "Another player already playing (" << state->plid << ")"
							<< std::endl;
		return true;
	}

	std::string request = createPacket(CMD_DEBUG, client_args, *state, true);
	std::string reply = createPacket(CMD_DEBUG, client_args, *state, false);

	Packet packet =
			sendUdpAndWait(state->udp_fd, request, reply, *state->server_addr);
	if (packet == NOT_SENT or packet == NOT_REPLIED)
		return true;
	else if (packet == INTERRUPT)
		return false;

	if (!parseDBG(reply, server_args)) {
		std::cerr << "There was a problem with the reply received from the Game "
								 "Server\nClosing down the application..."
							<< std::endl;	 // DEBUG
		close(state->udp_fd);
		exit(EXIT_SUCCESS);
	}

	Status status = server_args->status;
	if (status == NOK) {
		std::cout << "Game already in progress" << std::endl;
	} else if (status == OK) {
		std::cout << "New game started (max " << client_args.time
							<< " seconds) (secret key " << client_args.code.c_str() << ")"
							<< std::endl;
		state->playing = true;
		state->plid = client_args.plid;
		state->nT = 1;
	} else {
		std::cerr << "Message not recognized by the server" << std::endl;	 // DEBUG
	}

	return true;
}

bool runShowTrials(ServerArgs *server_args, ClientState *state) {
	if (!state->playing) {
		std::cout << "No game active" << std::endl;
		return true;
	}

	std::string request = createPacket(*state, true);
	std::string reply = createPacket(*state, false);
	if (!sendTcpAndWait(request, reply, state->ip, state->port)) return false;

	Packet packet = sendTcpAndWait(request, reply, state->ip, state->port);
	if (packet == NOT_SENT or packet == NOT_REPLIED)
		return true;
	else if (packet == INTERRUPT)
		return false;

	if (!parseRST(reply, server_args)) {
		std::cerr << "There was a problem with the reply received from the Game "
								 "Server\nClosing down the application..."
							<< std::endl;	 // DEBUG
		close(state->udp_fd);
		exit(EXIT_SUCCESS);
	}

	Status status = server_args->status;
	if (status == ACT) {
		// TODO save file locally and format output
		std::cout << "Trials file will be saved on " << server_args->fname << " ("
							<< server_args->fsize << "B)\n"
							<< server_args->fdata;
	} else if (status == FIN) {
		// TODO save file locally and format output
		std::cout << "Trials file will be saved on " << server_args->fname << " ("
							<< server_args->fsize << "B)\n"
							<< server_args->fdata;
		state->playing = false;
		state->plid.clear();
	} else {
		std::cout << "No file received" << std::endl;
		state->playing = false;
		state->plid.clear();
	}

	return true;
}

// bool runScoreboard(ServerArgs *server_args, ClientState *state) {
// 	std::string request = createPacket(CMD_SCOREBOARD, true);
// 	std::string reply = createPacket(CMD_SCOREBOARD, false);
// 	if (!sendTcpAndWait(request, reply, state->ip, state->port)) return false;

// 	Packet packet = sendTcpAndWait(request, reply, state->ip, state->port);
// 	if (packet == NOT_SENT or packet == NOT_REPLIED) return true;
// 	else if (packet == INTERRUPT) return false;

// 	if (!parseRSS(reply, server_args)) {
// 		std::cerr << "There was a problem with the reply received from the Game "
// 								 "Server\nClosing down the application..."
// 							<< std::endl;	 // DEBUG
// 		close(state->udp_fd);
// 		exit(EXIT_SUCCESS);
// 	}

// 	Status status = server_args->status;
// 	if (status == OK) {
// 		// TODO save file locally and format output
// 		std::cout << "Scoreboard file will be saved on " << server_args->fname <<
// " ("
// 							<< server_args->fsize << "B)\n"
// 							<< server_args->fdata;
// 	} else (status == EMPTY) {
// 		std::cout << "No game was yet won by any player" << std::endl;

// 	return true;
// }

// TODO
//  void runShowTrials(ServerArgs *server_args, ClientState *state) {
//  	if (!state->plid.size() or server_args->nW == 1000) {
//  		std::cerr << "There is no player active in the application.\n";
//  		return;
//  	}
//  	std::string packet(STR_LEN, ' ');
//  	sprintf(packet.data(), "STR %s\n", state->plid.c_str());
//  	std::string reply;
//  	struct addrinfo *addr;
//  	int fd = setTcpSocket(state->ip, state->port, &addr, false);
//  	aaa(fd, *addr, packet, reply);
//  	std::cout << reply;

// if (!sendUdpAndWait(state->fd, packet, reply, *state->addr, nullptr)) {
// 	std::cerr << "Game Server did not replied to the request.\n";
// 	return;
// }
// if (!parseQUT(reply, server_args)) {
// 	std::cerr << "Something wrong happened with the server.\n"
// 						<< "Closing the application ...";
// 	exit(1);
// }
// }

// void runScoreboard(ClientArgs args, ClientState *state) {

// }

/* ------------------------------------------------------------------------- */

std::string createPacket(Command cmd, ClientArgs args, ClientState state,
												 bool send) {
	std::string packet;
	if (cmd == CMD_START) {
		if (send) {
			packet.resize(sizeof("SNG PPPPPP TTT\n") - 1);
			sprintf(packet.data(), "SNG %s %.3d\n", args.plid.c_str(), args.time);
		} else
			packet.resize(sizeof("RSG SSS\n") - 1);
	} else if (cmd == CMD_TRY) {
		if (send) {
			packet.resize(sizeof("TRY PPPPPP C C C C t\n") - 1);
			sprintf(packet.data(), "TRY %s %s %d\n", state.plid.c_str(),
							args.code.c_str(), state.nT);
		} else
			packet.resize(sizeof("RTR SSS C C C C\n") - 1);
	} else {
		if (send) {
			packet.resize(sizeof("DBG PPPPPP TTT C C C C\n") - 1);
			sprintf(packet.data(), "DBG %s %3.d %s\n", state.plid.c_str(), args.time,
							args.code.c_str());
		} else
			packet.resize(sizeof("RDB SSS\n") - 1);
	}
	return packet;
}

std::string createPacket(ClientState state, bool send) {
	std::string packet;
	if (send) {
		packet.resize(sizeof("STR PPPPPP\n") - 1);
		sprintf(packet.data(), "STR %s\n", state.plid.c_str());
	}
	return packet;
}

std::string createPacket(Command cmd, bool send) {
	std::string packet;
	if (cmd == CMD_QUIT or cmd == CMD_EXIT) {
		if (send)
			packet = "QUT\n";
		else
			packet.resize(sizeof("RQT SS C C C C\n") - 1);
	} else {
		if (send) packet = "SSB\n";
	}
	return packet;
}
