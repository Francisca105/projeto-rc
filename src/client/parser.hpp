#ifndef PARSER_H
#define PARSER_H

#include "support.hpp"

// Master Mind colors
enum Colors {
	RED = 'R',
	GREEN = 'G',
	BLUE = 'B',
	YELLOW = 'Y',
	ORANGE = 'O',
	PURPLE = 'P'
};

Cmd getCmd(std::string buf);
bool parseCmd(std::string &buf, ClArgs *args, Cmd cmd);
bool parseStart(std::string buf, ClArgs *args);
bool parseTry(std::string buf, ClArgs *args);
bool parseDebug(std::string buf, ClArgs *args);

bool parseRSG(std::string buf, SvArgs *args);
bool parseRTR(std::string buf, SvArgs *args);
bool parseRQT(std::string buf, SvArgs *args);
bool parseRDB(std::string buf, SvArgs *args);
bool parseRST(std::string buf, SvArgs *args);
bool parseRSS(std::string buf, SvArgs *args);

/* ------------------------------------------------------------------------- */

bool parsePlid(std::string &buf, std::string &plid);
bool parsePlid(std::string &buf, std::vector<std::string> &vec);
bool parseTime(std::string &buf, int *time);
bool parseCode(std::string &buf, std::string &code);
bool parseCode(std::string &buf, std::vector<std::string> &codes);
bool parseSpace(std::string &buf);
void rtrim(std::string &buf);
bool parseStatus(std::string &buf, Status *status);
bool parseGuess(std::string &buf, int *nT, int *nB, int *nW);
bool parseNewline(std::string &buf);
bool parseDigit(std::string &buf, char min, char max, std::vector<char> &vec);
bool parseFileName(std::string &buf, std::string &filename);
bool parseFileSize(std::string &buf, int *file_size);

#endif
