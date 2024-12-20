#include "parser.hpp"

#include <cstdlib>

Cmd getCmd(std::string buf) {
	switch (buf.front()) {
		case 'S':
			if (!buf.compare(0, CMD_LEN, "SNG"))
				return Start;
			else if (!buf.compare(0, CMD_LEN, "STR"))
				return Showtrials;
			else if (!buf.compare(0, CMD_LEN, "SSB"))
				return Scoreboard;
			break;
		case 'T':
			if (!buf.compare(0, CMD_LEN, "TRY")) return Try;
			break;
		case 'Q':
			if (!buf.compare(0, CMD_LEN, "QUT")) return Quit;
			break;
		case 'D':
			if (!buf.compare(0, CMD_LEN, "DBG")) return Debug;
			break;
		default:
			break;
	}
	return Invalid;
}

bool parseCmd(std::string buf, Cmd cmd, Args *args) {
	buf.erase(0, CMD_LEN);
	switch (cmd) {
		case Start:
			if (!parseSNG(buf, args->plid, &args->time)) return false;
			break;
		case Try:
			if (!parseTRY(buf, args->plid, args->code, &args->nT)) return false;
			break;
		case Debug:
			if (!parseDBG(buf, args->plid, &args->time, args->code)) return false;
			break;
		case Quit:
		case Showtrials:
			if (!parseSpace(buf)) return false;
			if (!parsePlid(buf, args->plid)) return false;
			if (!parseNewline(buf)) return false;
			if (buf.size() != 0) return false;
			break;
		case Scoreboard:
			if (!parseNewline(buf)) return false;
			if (buf.size() != 0) return false;
			break;
		case Invalid:
		default:
			return false;
	}
	return true;
}

bool parseSNG(std::string buf, std::string &plid, int *time) {
	if (!parseSpace(buf)) return false;
	if (!parsePlid(buf, plid)) return false;
	if (!parseSpace(buf)) return false;
	if (!parseTime(buf, time)) return false;
	if (!parseNewline(buf)) return false;
	if (buf.size() != 0) return false;
	return true;
}

bool parseTRY(std::string buf, std::string &plid, std::string &code, int *nT) {
	if (!parseSpace(buf)) return false;
	if (!parsePlid(buf, plid)) return false;
	if (!parseSpace(buf)) return false;
	if (!parseCode(buf, code)) return false;
	if (!parseSpace(buf)) return false;
	if (!parseNT(buf, nT)) return false;
	if (!parseNewline(buf)) return false;
	if (buf.size() != 0) return false;
	return true;
}

bool parseDBG(std::string buf, std::string &plid, int *time, std::string &code) {
	if (!parseSpace(buf)) return false;
	if (!parsePlid(buf, plid)) return false;
	if (!parseSpace(buf)) return false;
	if (!parseTime(buf, time)) return false;
	if (!parseSpace(buf)) return false;
	if (!parseCode(buf, code)) return false;
	if (!parseNewline(buf)) return false;
	if (buf.size() != 0) return false;
	return true;
}

/* ------------------------------------------------------------------------- */

bool parsePlid(std::string &buf, std::string &plid) {
	if (!buf.size()) return false;
	size_t i = 0;
	char *ptr = buf.data(), c;
	while ((c = *ptr) != '\0') {
		if (!std::isdigit(c)) break;
		i++;
		ptr++;
	}
	if (i != PLID_LEN) return false;
	plid = buf.substr(0, PLID_LEN);
	buf.erase(0, PLID_LEN);
	return true;
}

bool parseTime(std::string &buf, int *time) {
	if (!buf.size()) return false;
	size_t i = 0;
	char *ptr = buf.data(), c;
	while ((c = *ptr) != '\0') {
		if (!std::isdigit(c)) break;
		i++;
		ptr++;
	}
	if (i != TIME_LEN) return false;
	char *end_ptr{};
	errno = 0;
	long tmp = std::strtol(buf.substr(0, i).c_str(), &end_ptr, 10);
	if (tmp == 0)
		return false;
	else if (errno == ERANGE or tmp < 0 or tmp > MAX_TIME)
		return false;
	*time = (int)tmp;
	buf.erase(0, i);
	return true;
}

bool parseCode(std::string &buf, std::string &code) {
	if (buf.size() < CODE_LEN) return false;
	std::string tmp = buf;
	char *ptr = tmp.data(), c;
	for (int i = 0; i < KEY_SIZE; i++) {
		c = *ptr;
		switch (c) {
			case 'R':
			case 'G':
			case 'B':
			case 'Y':
			case 'O':
			case 'P':
				break;
			default:
				return false;
		}
		tmp.erase(0, 1);
		if (i < KEY_SIZE - 1 and !parseSpace(tmp)) return false;
	}
	code = buf.substr(0, CODE_LEN);
	buf.erase(0, CODE_LEN);
	return true;
}

bool parseNT(std::string &buf, int *nT) {
	if (!buf.size() or !std::isdigit(buf.front())) return false;
	if (buf.front() < '1' or buf.front() > '0' + MAX_TRIES) return false;
	*nT = buf.front() - '0';
	buf.erase(0, 1);
	return true;
}

bool parseSpace(std::string &buf) {
	if (!buf.size() or buf.front() != ' ') return false;
	buf.erase(0, 1);
	return true;
}

bool parseNewline(std::string &buf) {
	if (!buf.size() or buf.front() != '\n') return false;
	buf.erase(0, 1);
	return true;
}

std::string cmdAsStr(Cmd cmd) {
	switch (cmd) {
		case Start:
			return "SNG";
		case Try:
			return "TRY";
		case Quit:
			return "QUT";
		case Debug:
			return "DBG";
		case Showtrials:
			return "STR";
		case Scoreboard:
			return "SSB";
		case Invalid:
		default:
			return "ERR";
	}
}
