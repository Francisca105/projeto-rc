#ifndef CLIENT_GAME_H
#define CLIENT_GAME_H

#include <string>

#include "client_state.hpp"

typedef struct {
	std::string plid;
	int time;
	std::string code;
} ClientArgs;

enum Command {
	CMD_START,
	CMD_TRY,
	CMD_QUIT,
	CMD_EXIT,
	CMD_DEBUG,
	CMD_SHOWTRIALS,
	CMD_SCOREBOARD,
	CMD_ERR
};

typedef struct {
	Status status;
	int nT;
	int nB;
	int nW;
	std::string code;
	std::string fname;
	int fsize;
	std::string fdata;
} ServerArgs;

void runCmd(Command cmd, ClientArgs client_args, ServerArgs *server_args,
						ClientState *state);
void runStart(ClientArgs client_args, ServerArgs *server_args,
							ClientState *state);
void runTry(ClientArgs client_args, ClientState *state);
void runQuit(ClientArgs client_args, ClientState *state);
void runExit(ClientArgs client_args, ClientState *state);
void runDebug(ClientArgs client_args, ClientState *state);
void runShowTrials(ClientArgs client_args, ClientState *state);
void runScoreboard(ClientArgs client_args, ClientState *state);

#endif
