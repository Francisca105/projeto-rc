#include "client.hpp"

#include <fstream>
#include <iostream>

#include "config.hpp"
#include "parser.hpp"
#include "protocol.hpp"

bool runCmd(Cmd cmd, ClArgs cl_args, SvArgs sv_args, Config config,
						State *state) {
	switch (cmd) {
		case Start:
			return runStart(cl_args, sv_args, config, state);
		case Try:
			return runTry(cl_args, sv_args, config, state);
			break;
		case Quit:
			return runQuit(sv_args, config, state);
			break;
		case Exit:
			runQuit(sv_args, config, state);
			KeepRunning = false;
			return true;
		case Debug:
			return runDebug(cl_args, sv_args, config, state);
		case Showtrials:
			return runShowTrials(sv_args, config, state);
		case Scoreboard:
			return runScoreboard(sv_args, config);
		case Invalid:
		default:
			break;
	}
	return true;
}

bool runStart(ClArgs cl_args, SvArgs sv_args, Config config, State *state) {
	if (state->playing) {
		std::cout << "Game already in progress" << std::endl;
		return true;
	}

	std::string request = createRequest(Start, cl_args);
	std::string reply = createReply(Start);
	if (!sendUdpAndReceive(request, reply, config.udp_fd, *config.server_addr))
		return true;

	if (!parseRSG(reply, &sv_args)) {
		std::cerr << "Server sent a wrongly formatted message" << std::endl;
		return false;
	}

	Status status = sv_args.status;
	if (status == NOK) {
		std::cerr << "Game already in progress" << std::endl;
	} else if (status == OK) {
		std::cout << "New game started (max " << cl_args.time << " seconds)"
							<< std::endl;
		state->playing = true;
		state->plid = cl_args.plid;
		state->nT = 1;
	} else {
		std::cerr << "Message not recognized by the server" << std::endl;
	}

	return true;
}

bool runTry(ClArgs cl_args, SvArgs sv_args, Config config, State *state) {
	if (!state->playing) {
		std::cout << "There is no active game" << std::endl;
		return true;
	}

	std::string request = createRequest(Try, cl_args, *state);
	std::string reply = createReply(Try);
	if (!sendUdpAndReceive(request, reply, config.udp_fd, *config.server_addr))
		return true;

	if (!parseRTR(reply, &sv_args)) {
		std::cerr << "Server sent a wrongly formatted message" << std::endl;
		return false;
	}

	Status status = sv_args.status;
	if (status == OK) {
		if (sv_args.nB == 4) {
			std::cout << "Well done! You guessed the key in " << sv_args.nT
								<< " trials" << std::endl;
			state->playing = false;
			state->plid.clear();
		} else {
			std::cout << "nB = " << sv_args.nB << ", nW = " << sv_args.nW
								<< std::endl;
			state->nT = sv_args.nT + 1;
		}
	} else if (status == DUP) {
		std::cout << "That was a repeat of a previous trial" << std::endl;
	} else if (status == INV) {
		std::cerr << "[DEBUG] Server replied with INV" << std::endl;
	} else if (status == NOK) {
		std::cerr << "There is no game in progress" << std::endl;
	} else if (status == ENT) {
		std::cout << "Oops! You ran out of tries. The secret key was: "
							<< sv_args.code << std::endl;
		state->playing = false;
		state->plid.clear();
	} else if (status == ETM) {
		std::cout << "Oops! You ran out of time. The secret key was: "
							<< sv_args.code << std::endl;
		state->playing = false;
		state->plid.clear();
	} else {
		std::cerr << "Message not recognized by the server" << std::endl;
	}

	return true;
}

bool runQuit(SvArgs sv_args, Config config, State *state) {
	if (!state->playing) {
		std::cout << "There is no active game" << std::endl;
		return true;
	}

	std::string request = createRequest(Quit, *state);
	std::string reply = createReply(Quit);
	if (!sendUdpAndReceive(request, reply, config.udp_fd, *config.server_addr))
		return true;

	if (!parseRQT(reply, &sv_args)) {
		std::cerr << "Server sent a wrongly formatted message" << std::endl;
		return false;
	}

	Status status = sv_args.status;
	if (status == OK) {
		std::cout << "Game ended. The secret key was: " << sv_args.code
							<< std::endl;
		state->playing = false;
		state->plid.clear();
	} else if (status == NOK) {
		std::cout << "There was not a game in progress" << std::endl;
		state->playing = false;
		state->plid.clear();
	} else {
		std::cerr << "Message not recognized by the server" << std::endl;
	}

	return true;
}

bool runDebug(ClArgs cl_args, SvArgs sv_args, Config config, State *state) {
	if (state->playing) {
		std::cout << "Game already in progress" << std::endl;
		return true;
	}

	std::string request = createRequest(Debug, cl_args);
	std::string reply = createReply(Debug);
	if (!sendUdpAndReceive(request, reply, config.udp_fd, *config.server_addr))
		return true;

	if (!parseRDB(reply, &sv_args)) {
		std::cerr << "Server sent a wrongly formatted message" << std::endl;
		return false;
	}

	Status status = sv_args.status;
	if (status == NOK) {
		std::cerr << "Game already in progress" << std::endl;
	} else if (status == OK) {
		std::cout << "New game started (max " << cl_args.time << " seconds)"
							<< "(secret key " << cl_args.code.c_str() << ")" << std::endl;
		state->playing = true;
		state->plid = cl_args.plid;
		state->nT = 1;
	} else {
		std::cerr << "Message not recognized by the server" << std::endl;
	}

	return true;
}

bool runShowTrials(SvArgs sv_args, Config config, State *state) {
	if (!state->playing) {
		std::cout << "There is no active game" << std::endl;
		return true;
	}

	std::string request = createRequest(Showtrials, *state);
	std::string reply;
	if (!sendTcpAndReceive(request, reply, config.ip, config.port)) return true;

	if (!parseRST(reply, &sv_args)) {
		std::cerr << "Server sent a wrongly formatted message" << std::endl;
		return false;
	}

	Status status = sv_args.status;
	if (status == ACT) {
		saveTrials(sv_args.trials);
		std::cout << "received trials file:\n" << sv_args.trials.data;
	} else if (status == FIN) {
		saveTrials(sv_args.trials);
		std::cout << "received trials file:\n" << sv_args.trials.data;
		state->playing = false;
		state->plid.clear();
	} else {
		std::cout << "No file received" << std::endl;
		state->playing = false;
		state->plid.clear();
	}

	return true;
}

bool runScoreboard(SvArgs sv_args, Config config) {
	std::string request = "SSB\n";
	std::string reply;
	if (!sendTcpAndReceive(request, reply, config.ip, config.port)) return true;
	if (!parseRSS(reply, &sv_args)) {
		std::cerr << "Server sent a wrongly formatted message" << std::endl;
		return false;
	}

	Status status = sv_args.status;
	if (status == OK) {
		saveScoreboard(sv_args.scoreboard);
		std::cout << sv_args.scoreboard.data;
	} else {
		std::cout << "No game was yet won by a player" << std::endl;
	}

	return true;
}

/* ------------------------------------------------------------------------- */

std::string createRequest(Cmd cmd, State state) {
	std::string request;
	if (cmd == Quit) {
		request.resize(sizeof("QUT PPPPPP\n") - 1);
		sprintf(request.data(), "QUT %s\n", state.plid.c_str());
	} else {
		request.resize(sizeof("STR PPPPPP\n") - 1);
		sprintf(request.data(), "STR %s\n", state.plid.c_str());
	}
	return request;
}

std::string createRequest(Cmd cmd, ClArgs args) {
	std::string request;
	if (cmd == Start) {
		request.resize(sizeof("SNG PPPPPP TTT\n") - 1);
		sprintf(request.data(), "SNG %s %03d\n", args.plid.c_str(), args.time);
	} else {
		request.resize(sizeof("DBG PPPPPP TTT C C C C\n") - 1);
		sprintf(request.data(), "DBG %s %03d %s\n", args.plid.c_str(), args.time,
						args.code.c_str());
	}
	return request;
}

std::string createRequest(Cmd cmd, ClArgs args, State state) {
	std::string request;
	if (cmd == Try) {
		request.resize(sizeof("TRY PPPPPP C C C C n\n") - 1);
		sprintf(request.data(), "TRY %s %s %d\n", state.plid.c_str(),
						args.code.c_str(), state.nT);
	}
	return request;
}

// Creates space for the biggest reply possible for each command
std::string createReply(Cmd cmd) {
	std::string request;
	if (cmd == Start) {
		request.resize(sizeof("RSG SSS\n") - 1);
	} else if (cmd == Try) {
		request.resize(sizeof("RTR SSS C C C C\n") - 1);
	} else if (cmd == Quit) {
		request.resize(sizeof("RQT SS C C C C\n") - 1);
	} else if (cmd == Debug) {
		request.resize(sizeof("RDB SSS\n") - 1);
	}
	return request;
}

bool saveTrials(Trials trials) {
	std::ofstream file(TRIALS_DIR + trials.fname);
	if (file.is_open()) {
		file << trials.data;
		if (file.fail()) {
			std::cerr << "Failed to save file" << std::endl;
			return false;
		}
		file.close();
		std::cout << "Trials file saved as \"" << trials.fname << "\" ("
							<< trials.fsize << " bytes)" << std::endl;
		return true;
	} else {
		std::cerr << "Failed to save file" << std::endl;
		return false;
	}
}

bool saveScoreboard(ScoreBoard scoreboard) {
	std::ofstream file(SCOREBOARDS_DIR + scoreboard.fname);
	if (file.is_open()) {
		file << scoreboard.data;
		if (file.fail()) {
			std::cerr << "Failed to save file" << std::endl;
			return false;
		}
		file.close();
		std::cout << "Scoreboard file saved as \"" << scoreboard.fname << "\" ("
							<< scoreboard.fsize << " bytes)" << std::endl;
		return true;
	} else {
		std::cerr << "Failed to save file" << std::endl;
		return false;
	}
}
