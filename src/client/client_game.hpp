#ifndef CLIENT_GAME_H
#define CLIENT_GAME_H

#include <string>
#include <set>

struct Input {
	std::string plid = "";
	char code[4];
	int time = 0;
};

const std::set<char> Colors = {'R', 'G', 'B', 'Y', 'O', 'P'};

enum Command {
	CMD_START,
	CMD_TRY,
	CMD_QUIT,
	CMD_EXIT,
	CMD_DEBUG,
	CMD_ST,
	CMD_SB,
	CMD_ERR
};

Command getCmd(std::string line);
void parseCmd(std::string line, Input *input, Command cmd);
bool parseStart(std::string s, Input *input);

bool parsePlid(std::string &s, Input *input);
bool parseTime(std::string &s, Input *input);
bool parseCode(std::string &s, Input *input);
bool parseSpace(std::string &s);

#endif
