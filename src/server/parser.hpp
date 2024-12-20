#ifndef PARSER_H
#define PARSER_H

#include <string>

#define CMD_LEN 3
#define PLID_LEN 6
#define TIME_LEN 3
#define CODE_LEN 7

#define KEY_SIZE 4

#define MAX_TIME 600

#define MAX_TRIES 8

// Client/Server commands
enum Cmd { Start, Try, Quit, Debug, Showtrials, Scoreboard, Invalid };

// Client args
typedef struct {
	std::string plid;
	int time = 0;
	std::string code;
	int nT = 0;
} Args;

Cmd getCmd(std::string buf);
bool parseCmd(std::string buf, Cmd cmd, Args *args);
bool parseSNG(std::string buf, std::string &plid, int *time);
bool parseTRY(std::string buf, std::string &plid, std::string &code, int *nT);
bool parseDBG(std::string buf, std::string &plid, int *time, std::string &code);

/* ------------------------------------------------------------------------- */

bool parsePlid(std::string &buf, std::string &plid);
bool parseTime(std::string &buf, int *time);
bool parseCode(std::string &buf, std::string &code);
bool parseNT(std::string &buf, int *nT);
bool parseSpace(std::string &buf);
bool parseNewline(std::string &buf);
std::string cmdAsStr(Cmd cmd);

#endif
