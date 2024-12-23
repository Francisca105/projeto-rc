#ifndef PARSER_H
#define PARSER_H

#include "support.hpp"

#define CMD_LEN 3
#define CODE_LEN 7
#define PLID_LEN 6
#define TIME_LEN 3

#define MAX_TIME 600
#define NUM_COLORS 4
#define MAX_TRIES 8

#define MAX_FILENAME 24

// Client/Server commands
enum Cmd { Start, Try, Quit, Exit, Debug, Showtrials, Scoreboard, Invalid };

// Master Mind colors
enum Colors {
	RED = 'R',
	GREEN = 'G',
	BLUE = 'B',
	YELLOW = 'Y',
	ORANGE = 'O',
	PURPLE = 'P'
};

// Client args
typedef struct {
	std::string plid;
	int time = 0;
	std::string code;
} ClArgs;

// ShowTrials server args
typedef struct {
	std::string fname;
	int fsize = 0;
	std::string data;
} Trials;

typedef struct {
	std::string fname;
	int fsize = 0;
	std::string data;
} ScoreBoard;

// Server args
typedef struct {
	Status status;
	int nT = 0;
	int nB = -1;
	int nW = -1;
	std::string code;
	Trials trials;
	ScoreBoard scoreboard;
} SvArgs;

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
