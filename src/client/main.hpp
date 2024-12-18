#ifndef MAIN_H
#define MAIN_H

#include <string>

#include "support.hpp"

bool handleCmd(std::string &buf, ClArgs cl_args, SvArgs sv_args, Config config,
							 State *state);
void sigintHandler(int signal);
bool isOnlyWhiteSpace(std::string buf);

#endif
