#ifndef CLIENT_H
#define CLIENT_H

#include <string>

#include "client_game.hpp"
#include "client_state.hpp"

void sigintHandler(int signal);
void setSignal(int signal, void (*func)(int));
bool isOnlyWhiteSpace(std::string s);
void handleCmd(std::string &s, ClientArgs *client_args, ServerArgs *server_args,
							 ClientState *state);

#endif
