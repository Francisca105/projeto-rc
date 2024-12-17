#ifndef CLIENT_H
#define CLIENT_H

#include <string>

#include "client_game.hpp"
#include "client_state.hpp"

bool handleCmd(std::string &buf, ClientArgs *client_args,
							 ServerArgs *server_args, ClientState *state);
void sigintHandler(int signal);
bool isOnlyWhiteSpace(std::string buf);

#endif
