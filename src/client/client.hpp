#ifndef CLIENT_H
#define CLIENT_H

#include "support.hpp"

bool runCmd(Cmd cmd, ClArgs cl_args, SvArgs sv_args, Config config,
						State *state);
bool runStart(ClArgs cl_args, SvArgs sv_args, Config config, State *state);
bool runTry(ClArgs cl_args, SvArgs sv_args, Config config, State *state);
bool runQuit(SvArgs sv_args, Config config, State *state);
bool runDebug(ClArgs cl_args, SvArgs sv_args, Config config, State *state);
bool runShowTrials(SvArgs sv_args, Config config, State *state);
bool runScoreboard(SvArgs sv_args, Config config);

/* ------------------------------------------------------------------------- */

std::string createRequest(Cmd cmd, State state);
std::string createRequest(Cmd cmd, ClArgs args);
std::string createRequest(Cmd cmd, ClArgs args, State state);
std::string createReply(Cmd cmd);
bool saveTrials(Trials trials);
bool saveScoreboard(ScoreBoard scoreboard);

#endif
