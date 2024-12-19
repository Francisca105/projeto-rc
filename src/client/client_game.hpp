#ifndef CLIENT_GAME_H
#define CLIENT_GAME_H

#include <unistd.h>

#include <string>

#include "support.hpp"

bool runCmd(Command cmd, ClientArgs client_args, ServerArgs *server_args,
						ClientState *state);
bool runStart(ClientArgs client_args, ServerArgs *server_args,
							ClientState *state);
bool runTry(ClientArgs client_args, ServerArgs *server_args,
						ClientState *state);
bool runQuit(ServerArgs *server_args, ClientState *state);
bool runExit(ClientArgs client_args, ClientState *state);
bool runDebug(ClientArgs client_args, ServerArgs *server_args,
							ClientState *state);
bool runShowTrials(ServerArgs *server_args, ClientState *state);
bool runScoreboard(ClientArgs client_args, ClientState *state);
/* ------------------------------------------------------------------------- */
std::string createPacket(Command cmd, ClientArgs args, ClientState state,
												 bool send);
std::string createPacket(ClientState state, bool send);
std::string createPacket(Command cmd, bool send);

#endif
