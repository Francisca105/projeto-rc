#ifndef CLIENT_PARSER_H
#define CLIENT_PARSER_H

#include "support.hpp"

Command getCmd(std::string line);
bool parseCmd(std::string &line, ClientArgs *args, Command cmd);
bool parseStart(std::string buf, ClientArgs *args);
bool parseTry(std::string buf, ClientArgs *args);
bool parseDebug(std::string buf, ClientArgs *args);
/* ------------------------------------------------------------------------- */
bool parseRSG(std::string buf, ServerArgs *args);
bool parseRTR(std::string buf, ServerArgs *args);
bool parseQUT(std::string buf, ServerArgs *args);
bool parseDBG(std::string buf, ServerArgs *args);
bool parseRST(std::string buf, ServerArgs *args);
/* ------------------------------------------------------------------------- */
bool parsePlid(std::string &buf, std::string &plid);
bool parseTime(std::string &buf, int *time);
bool parseCode(std::string &buf, std::string &code);
bool parseSpace(std::string &buf);
void rtrim(std::string &buf);
/* ------------------------------------------------------------------------- */
bool parseStatus(std::string &buf, Status *status);
bool parseGuess(std::string &buf, int *nT, int *nB, int *nW);
bool parseNewline(std::string &buf);
bool parseFileName(std::string &buf, std::string &filename);
bool parseFileSize(std::string &buf, int *file_size);

#endif
