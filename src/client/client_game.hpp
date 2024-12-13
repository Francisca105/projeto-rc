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

void runCmd(Command cmd, ClientArgs args, ClientState *state);
void runStart(ClientArgs args, ClientState *state);
void runTry(ClientArgs args, ClientState *state);
void runQuit(ClientArgs args, ClientState *state);
void runExit(ClientArgs args, ClientState *state);
void runDebug(ClientArgs args, ClientState *state);
void runShowTrials(ClientArgs args, ClientState *state);
void runScoreboard(ClientArgs args, ClientState *state);

#endif
