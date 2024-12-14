#ifndef CLIENT_PARSER_H
#define CLIENT_PARSER_H

#include "client_game.hpp"

Command getCmd(std::string line);
bool parseCmd(std::string &line, ClientArgs *args, Command cmd);
bool parseStart(std::string buf, ClientArgs *args);
bool parseTry(std::string buf, ClientArgs *args);
bool parseDebug(std::string buf, ClientArgs *args);

bool parsePlid(std::string &buf, std::string &plid);
bool parseTime(std::string &buf, int *time);
bool parseCode(std::string &buf, std::string &code);
bool parseSpace(std::string &buf);
bool parseNewline(std::string &buf);
void rtrim(std::string &buf);

bool parseRSG(std::string buf, ServerArgs *args);
bool parseStatus(std::string &buf, Status *status);
bool parseRTR(std::string buf, ServerArgs *args);
bool parseXXX(std::string &buf, int *nT, int *nB, int *nW);
bool parseQUT(std::string buf, ServerArgs *args);
bool parseDBG(std::string buf, ServerArgs *args);
#endif
