#include "parser.hpp"

#include "game.hpp"

#include "../common/common.hpp"

Command getCmd(std::string buf, int len) {
	if (len < 3) return CMD_ERR;
	switch (buf[0]) {
		case 'S':
			if (buf.compare(0, CMD_LEN, "SNG") == 0) return CMD_SNG;
			return CMD_ERR;
		case 'T':
			if (buf.compare(0, CMD_LEN, "TRY") == 0) return CMD_TRY;
			return CMD_ERR;
		case 'Q':
			if (buf.compare(0, CMD_LEN, "QUT") == 0) return CMD_QUT;
			return CMD_ERR;
		case 'D':
			if (buf.compare(0, CMD_LEN, "DBG") == 0) return CMD_DBG;
			return CMD_ERR;
		default:
			return CMD_ERR;
	}
}

bool parseSng(std::string buf, int len, std::string &plid, int *time) {
	std::string tmp = buf;
	if (len == 0) return false;
	if (parseSpace(tmp, &len) == false) return false;
	if (parsePlid(tmp, &len, plid) == false) return false;
	if (parseSpace(tmp, &len) == false) return false;
	if (parseTime(tmp, &len, time) == false) return false;
	if (parseNewline(tmp, &len) == false) return false;
	return true;
}

bool parseTry(std::string buf, int len, std::string &plid, std::string &code,
							int *nt) {
	std::string tmp = buf;
	if (len == 0) return false;
	if (parseSpace(tmp, &len) == false) return false;
	if (parsePlid(tmp, &len, plid) == false) return false;
	if (parseSpace(tmp, &len) == false) return false;
	if (parseCode(tmp, &len, code) == false) return false;
	if (parseNt(tmp, &len, nt) == false) return false;
	if (parseNewline(tmp, &len) == false) return false;
	return true;
}

bool parseQut(std::string buf, int len, std::string &plid) {
	std::string tmp = buf;
	if (len == 0) return false;
	if (parseSpace(tmp, &len) == false) return false;
	if (parsePlid(tmp, &len, plid) == false) return false;
	if (parseNewline(tmp, &len) == false) return false;
	return true;
}

bool parseDbg(std::string buf, int len, std::string &plid, int *time,
							std::string &code) {
	std::string tmp = buf;
	if (len == 0) return false;
	if (parseSpace(tmp, &len) == false) return false;
	if (parsePlid(tmp, &len, plid) == false) return false;
	if (parseSpace(tmp, &len) == false) return false;
	if (parseTime(tmp, &len, time) == false) return false;
	if (parseSpace(tmp, &len) == false) return false;
	if (parseCode(tmp, &len, code) == false) return false;
	if (parseNewline(tmp, &len) == false) return false;
	return true;
}

bool parsePlid(std::string &s, int *n, std::string &plid) {
	if (*n < PLID_LEN) return false;
	int count = 0;
	for (size_t i = 0; i < (size_t)*n; i++, count++)
		if (std::isdigit(s.at(i)) == 0) break;
	if (count != PLID_LEN) return false;
	plid = s.substr(0, PLID_LEN);
	*n = *n - PLID_LEN;
	s.erase(0, PLID_LEN);
	return true;
}

bool parseTime(std::string &s, int *n, int *time) {
	if (*n < TIME_LEN) return false;
	int count = 0;
	for (size_t i = 0; i < (size_t)*n; i++, count++)
		if (std::isdigit(s.at(i)) == 0) break;
	if (count != TIME_LEN) return false;
	try {
		int tmp = std::stoi(s.substr(0, TIME_LEN));
		if (tmp <= 0 or tmp > 600) return false;
		*time = tmp;
	} catch (std::exception const &ex) {
		return false;
	}
	*n = *n - TIME_LEN;
	s.erase(0, TIME_LEN);
	return true;
}

bool parseCode(std::string &buf, int *n, std::string &code) {
	if (buf.size() < CODE_LEN) return false;
	std::string tmp = buf;
	char *ptr = tmp.data(), c;
	for (int i = 0; i < NUM_COLORS; i++) {
		c = *ptr;
		switch (c) {
			case RED:
			case GREEN:
			case BLUE:
			case YELLOW:
			case ORANGE:
			case PURPLE:
				break;
			default:
				return false;
		}
		tmp.erase(0, 1);
		if (i < 3 and !parseSpace(tmp, n)) return false;
	}
	code = buf.substr(0, CODE_LEN);
	buf.erase(0, CODE_LEN);

	std::cout << "Code: " << code << std::endl;
	return true;
}

bool parseColor(std::string &s, int *n) {
	if (*n == 0) return false;
	// Check if the first character is a valid color
	if(!GameUtils::isColorValid(s[0])) return false;

	*n = *n - 1;
	s.erase(0, 1);
	return true;
}

bool parseNt(std::string &s, int *n, int *nt) {
	if (*n < 1) return false;
	int count = 0;
	for (size_t i = 0; i < (size_t)*n; i++, count++)
		if (std::isdigit(s.at(i)) == 0) break;
	if (count != 1) return false;
	try {
		int tmp = std::stoi(s.substr(0, 1));
		if (tmp <= 0 or tmp > MAX_TRIALS) return false;
		*nt = tmp;
	} catch (std::exception const &ex) {
		return false;
	}
	*n = *n - 1;
	s.erase(0, 1);
	return true;
}

bool parseSpace(std::string &s, int *n) {
	if (*n == 0) return false;
	if (s[0] != ' ') return false;
	*n = *n - 1;
	s.erase(0, 1);
	return true;
}

bool parseNewline(std::string &s, int *n) {
	if (*n == 0) return false;
	if (s[0] != '\n') return false;
	*n = *n - 1;
	s.erase(0, 1);
	return true;
}
