#ifndef PARSER_H
#define PARSER_H

#include "aux.hpp"

#define CMD_LEN 3
#define PLID_LEN 6
#define TIME_LEN 3
#define CODE_LEN 7

Command getCmd(std::string buf, int len);
bool parseSng(std::string buf, int len, std::string &plid, int *time);
bool parseTry(std::string buf, int len, std::string &plid, std::string &code,
							int *nt);
bool parseQut(std::string buf, int len, std::string &plid);
bool parseDbg(std::string buf, int len, std::string &plid, int *time,
							std::string &code);

bool parsePlid(std::string &s, int *n, std::string &plid);
bool parseTime(std::string &s, int *n, int *time);
bool parseCode(std::string &s, int *n, std::string &code);
bool parseColor(std::string &s, int *n);
bool parseNt(std::string &s, int *n, int *nt);
bool parseSpace(std::string &s, int *n);
bool parseNewline(std::string &s, int *n);

#endif
