#include "client_game.hpp"

#include <cstring>
#include <iostream>

#include "../common/common.hpp"

Command getCmd(std::string line) {
	std::string first = line.substr(0, line.find(' '));
	switch (line.front()) {
		case 's':
			if (!first.compare("start"))
				return CMD_START;
			else if (!first.compare("show_trials") or !first.compare("st"))
				return CMD_ST;
			else if (!first.compare("scoreborad") or !first.compare("sb"))
				return CMD_SB;
			break;
		case 't':
			if (!first.compare("try")) return CMD_TRY;
			break;
		case 'q':
			if (!first.compare("quit")) return CMD_QUIT;
			break;
		case 'e':
			if (!first.compare("exit")) return CMD_EXIT;
			break;
		case 'd':
			if (!first.compare("debug")) return CMD_DEBUG;
			break;
		default:
			return CMD_ERR;
	}
	return CMD_ERR;
}

void parseCmd(std::string line, Input *input, Command cmd) {
	std::string first = line.substr(0, line.find(' '));
	std::string s = line.size() > first.size() ? line.substr(line.find(' ')) : first;
	switch (cmd) {
		case CMD_START:
			if (!parseStart(s, input)) std::cout << "Error parsing start command.\n";
			break;
		case CMD_TRY:
			// parseTry(line);
			break;
		case CMD_QUIT:
			// parseQuit(line);
			break;
		case CMD_EXIT:
			// parseExit(line);
			break;
		case CMD_DEBUG:
			// parseDebug(line);
			break;
		case CMD_ST:
			// parseShowTrials(line);
			break;
		case CMD_SB:
			// parseScoreboard(line);
			break;
		case CMD_ERR:
		default:
			break;
	}
}

bool parseStart(std::string s, Input *input) {
	if (!parseSpace(s)) return false;
	if (!parsePlid(s, input)) return false;
	if (!parseSpace(s)) return false;
	if (!parseTime(s, input)) return false;
	if (!s.size()) return true;
	return false;
}

bool parseTry(std::string s, Input *input) {
	if (!parseSpace(s)) return false;
	if (!parseCode(s, input)) return false;
	if (!s.size()) return true;
	return false;
}

bool parsePlid(std::string &s, Input *input) {
	if (!s.size()) return false;
	int i = 0;
	char *ptr = s.data(), c;
	while ((c = *ptr) != '\0') {
		if (!std::isdigit(c)) break;
		i++;
		ptr++;
	}
	if (i != PLID_LEN) {
		std::cout << "Error parsing PLID.\n";
		return false;
	}
	input->plid = s.substr(0, PLID_LEN).data();
	s.erase(0, PLID_LEN);
	return true;
}

bool parseTime(std::string &s, Input *input) {
	if (!s.size()) return false;
	size_t i = 0;
	char *ptr = s.data(), c;
	while ((c = *ptr) != '\0') {
		if (!std::isdigit(c)) break;
		i++;
		ptr++;
	}
	int time = std::atoi(s.substr(0, TIME_LEN).c_str());
	if (time <= 0 or time > MAX_TIME) {
		std::cout << "Invalid playtime. Value must be between 1 and 600.\n";
		return false;
	}
	input->time = time;
	s.erase(0, i);
	return true;
}

bool parseCode(std::string &s, Input *input) {
	if (s.size() < CODE_LEN) return false;
	char *ptr = s.data(), c;
	for (int i=0; Colors.size() ;i++) {
		if ()
	}
}

bool parseSpace(std::string &s) {
	if (s.size() > 0 and s.front() != ' ') return false;
	s.erase(0, 1);
	return true;
}
