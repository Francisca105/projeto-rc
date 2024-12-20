#ifndef SERVER_H
#define SERVER_H

#include "config.hpp"
#include "game.hpp"
#include "parser.hpp"
#include "protocol.hpp"

#define MAX_REQUEST_LEN 24

void handleUdp(Config config);
void runCmd(Cmd cmd, Args args, Config config, Address addr);
void runStart(Args args, Config config, Address addr);
void runTry(Args args, Config config, Address addr);
void runDebug(Args args, Config config, Address addr);
void runQuit(Args args, Config config, Address addr);
void printVerbose(std::string plid, std::string request, Address addr);
void runShowTrials(Args args, Config config);
void runScoreboard(Config config);
void handleTcp(Config config);
void printVerbose(std::string plid, std::string request, int fd);
void printVerbose(std::string request, int fd)

#endif
