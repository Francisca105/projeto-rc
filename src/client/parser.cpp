#include "parser.hpp"

#include <algorithm>
#include <iostream>
#include <unordered_map>

// It is used to get the status as a string
std::unordered_map<std::string, Status> StatusDict = {
		{"OK", OK},		{"NOK", NOK}, {"ERR", ERR}, {"DUP", DUP}, {"INV", INV},
		{"ENT", ENT}, {"ETM", ETM}, {"ACT", ACT}, {"FIN", FIN}, {"EMPTY", EMPTY}};

Cmd getCmd(std::string buf) {
	std::string first = buf.substr(0, buf.find(' '));
	switch (first.front()) {
		case 's':
			if (!first.compare("start"))
				return Start;
			else if (!first.compare("show_trials") or !first.compare("st"))
				return Showtrials;
			else if (!first.compare("scoreboard") or !first.compare("sb"))
				return Scoreboard;
			break;
		case 't':
			if (!first.compare("try")) return Try;
			break;
		case 'q':
			if (!first.compare("quit")) return Quit;
			break;
		case 'e':
			if (!first.compare("exit")) return Exit;
			break;
		case 'd':
			if (!first.compare("debug")) return Debug;
			break;
		default:
			break;
	}
	return Invalid;
}

bool parseCmd(std::string &buf, ClArgs *args, Cmd cmd) {
	bool error = false;
	std::string first = buf.substr(0, buf.find(' '));
	std::string remaining =
			buf.size() == first.size() ? "" : buf.substr(buf.find(' '));
	switch (cmd) {
		case Start:
			if (!parseStart(remaining, args)) error = true;
			break;
		case Try:
			if (!parseTry(remaining, args)) error = true;
			break;
		case Debug:
			if (!parseDebug(remaining, args)) error = true;
			break;
		case Quit:
		case Exit:
		case Showtrials:
		case Scoreboard:
			rtrim(remaining);
			if (remaining.size() != 0) error = true;
			break;
		case Invalid:
		default:
			error = true;
	}
	if (error) buf = first;
	return error;
}

bool parseStart(std::string buf, ClArgs *args) {
	if (!parseSpace(buf)) return false;
	if (!parsePlid(buf, args->plid)) return false;
	if (!parseSpace(buf)) return false;
	if (!parseTime(buf, &args->time)) return false;
	rtrim(buf);
	if (buf.size() != 0) return false;
	return true;
}

bool parseTry(std::string buf, ClArgs *args) {
	if (!parseSpace(buf)) return false;
	if (!parseCode(buf, args->code)) return false;
	rtrim(buf);
	if (buf.size() != 0) return false;
	return true;
}

bool parseDebug(std::string buf, ClArgs *args) {
	if (!parseSpace(buf)) return false;
	if (!parsePlid(buf, args->plid)) return false;
	if (!parseSpace(buf)) return false;
	if (!parseTime(buf, &args->time)) return false;
	if (!parseSpace(buf)) return false;
	if (!parseCode(buf, args->code)) return false;
	rtrim(buf);
	if (buf.size() != 0) return false;
	return true;
}

/* ------------------------------------------------------------------------- */

bool parseRSG(std::string buf, SvArgs *args) {
	if (buf.size() < CMD_LEN or buf.substr(0, CMD_LEN).compare("RSG"))
		return false;
	buf.erase(0, CMD_LEN);
	if (!parseSpace(buf)) return false;
	if (!parseStatus(buf, &args->status)) return false;
	if (!parseNewline(buf)) return false;
	if (buf.size() != 0) return false;
	return true;
}

bool parseRTR(std::string buf, SvArgs *args) {
	if (buf.size() < CMD_LEN or buf.substr(0, CMD_LEN).compare("RTR"))
		return false;
	buf.erase(0, CMD_LEN);
	if (!parseSpace(buf)) return false;
	if (!parseStatus(buf, &args->status)) return false;
	Status status = args->status;
	if (status == OK) {
		if (!parseSpace(buf)) return false;
		if (!parseGuess(buf, &args->nT, &args->nB, &args->nW)) return false;

	} else if (status == ENT or status == ETM) {
		if (!parseSpace(buf)) return false;
		if (!parseCode(buf, args->code)) return false;
	}
	if (!parseNewline(buf)) return false;
	if (buf.size() != 0) return false;
	return true;
}

bool parseRQT(std::string buf, SvArgs *args) {
	if (buf.size() < CMD_LEN or buf.substr(0, CMD_LEN).compare("RQT"))
		return false;
	buf.erase(0, CMD_LEN);
	if (!parseSpace(buf)) return false;
	if (!parseStatus(buf, &args->status)) return false;
	Status status = args->status;
	if (status == OK) {
		if (!parseSpace(buf)) return false;
		if (!parseCode(buf, args->code)) return false;
	}
	if (!parseNewline(buf)) return false;
	if (buf.size() != 0) return false;
	return true;
}

bool parseRDB(std::string buf, SvArgs *args) {
	if (buf.size() < CMD_LEN or buf.substr(0, CMD_LEN).compare("RDB"))
		return false;
	buf.erase(0, CMD_LEN);
	if (!parseSpace(buf)) return false;
	if (!parseStatus(buf, &args->status)) return false;
	if (!parseNewline(buf)) return false;
	if (buf.size() != 0) return false;
	return true;
}

bool parseRST(std::string buf, SvArgs *args) {
	if (buf.size() < CMD_LEN or buf.substr(0, CMD_LEN).compare("RST"))
		return false;
	buf.erase(0, CMD_LEN);
	if (!parseSpace(buf)) return false;
	if (!parseStatus(buf, &args->status)) return false;
	if (args->status == NOK) {
		if (!parseNewline(buf)) return false;
	} else {
		if (!parseSpace(buf)) return false;
		if (!parseFileName(buf, args->trials.fname)) return false;
		if (!parseSpace(buf)) return false;
		if (!parseFileSize(buf, &args->trials.fsize)) return false;
		if (!parseSpace(buf)) return false;
		if (buf.size() != (size_t)args->trials.fsize) return false;
		// return parseTrialsData(buf, args);
		if (buf.back() != '\n') return false;
		args->trials.data = buf;
	}
	return true;
}

bool parseRSS(std::string buf, SvArgs *args) {
	if (buf.size() < CMD_LEN or buf.substr(0, CMD_LEN).compare("RSS"))
		return false;
	buf.erase(0, CMD_LEN);
	if (!parseSpace(buf)) return false;
	if (!parseStatus(buf, &args->status)) return false;
	if (args->status == EMPTY) {
		if (!parseNewline(buf)) return false;
	} else {
		if (!parseSpace(buf)) return false;
		if (!parseFileName(buf, args->scoreboard.fname)) return false;
		if (!parseSpace(buf)) return false;
		if (!parseFileSize(buf, &args->scoreboard.fsize)) return false;
		if (!parseSpace(buf)) return false;
		if (buf.size() != (size_t)args->scoreboard.fsize) return false;
		// return parseScoreboardData(buf, args);
		if (buf.back() != '\n') return false;
		args->scoreboard.data = buf;
	}
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

bool parsePlid(std::string &buf, std::vector<std::string> &vec) {
	if (!buf.size()) return false;
	size_t i = 0;
	char *ptr = buf.data(), c;
	while ((c = *ptr) != '\0') {
		if (!std::isdigit(c)) break;
		i++;
		ptr++;
	}
	if (i != PLID_LEN) return false;
	vec.push_back(buf.substr(0, PLID_LEN));
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
		if (i < 3 and !parseSpace(tmp)) return false;
	}
	code = buf.substr(0, CODE_LEN);
	buf.erase(0, CODE_LEN);
	return true;
}

bool parseCode(std::string &buf, std::vector<std::string> &codes) {
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
		if (i < 3 and !parseSpace(tmp)) return false;
	}
	codes.push_back(buf.substr(0, CODE_LEN));
	buf.erase(0, CODE_LEN);
	return true;
}

bool parseSpace(std::string &buf) {
	if (!buf.size() or buf.front() != ' ') return false;
	buf.erase(0, 1);
	return true;
}

void rtrim(std::string &buf) {
	buf.erase(std::find_if(buf.rbegin(), buf.rend(),
												 [](unsigned char ch) { return !std::isspace(ch); })
								.base(),
						buf.end());
}

bool parseStatus(std::string &buf, Status *status) {
	char *ptr = buf.data();
	size_t i;
	for (i = 0; i < buf.size(); i++, ptr++) {
		if (!isupper(*ptr)) break;
	}
	if (auto it = StatusDict.find(buf.substr(0, i)); it != StatusDict.end()) {
		*status = it->second;
		buf.erase(0, i);
		return true;
	}
	return false;
}

bool parseGuess(std::string &buf, int *nT, int *nB, int *nW) {
	if (buf.size() < sizeof("T B W")) return false;
	long nT_l = std::atol(buf.substr(0, 1).c_str());
	if (nT_l <= 0 or nT_l > MAX_TRIES - 1) return false;
	buf.erase(0, 1);
	if (!parseSpace(buf)) return false;
	long nB_l = std::atol(buf.substr(0, 1).c_str());
	if (nB_l == 0 and !std::isdigit(buf.substr(0, 1).c_str()[0])) return false;
	if (nB_l < 0 or nB_l > NUM_COLORS) return false;
	buf.erase(0, 1);
	if (!parseSpace(buf)) return false;
	long nW_l = std::atol(buf.substr(0, 1).c_str());
	if (nW_l == 0 and !std::isdigit(buf.substr(0, 1).c_str()[0])) return false;
	if (nW_l < 0 or nW_l > NUM_COLORS) return false;
	buf.erase(0, 1);
	*nT = (int)nT_l;
	*nB = (int)nB_l;
	*nW = (int)nW_l;
	return true;
}

bool parseNewline(std::string &buf) {
	if (!buf.size() or buf.front() != '\n') return false;
	buf.erase(0, 1);
	return true;
}

bool parseDigit(std::string &buf, char min, char max, std::vector<char> &vec) {
	if (!buf.size() or !std::isdigit(buf.front())) return false;
	if (buf.front() < min or buf.front() > max) return false;
	vec.push_back(buf.front());
	buf.erase(0, 1);
	return true;
}

bool parseFileName(std::string &buf, std::string &filename) {
	if (buf.size() < sizeof("n.xxx") - 1) return false;
	char *ptr = buf.data(), c;
	size_t i, max_len = std::min(buf.size(), (size_t)MAX_FILENAME);
	for (i = 0; i < max_len; i++, ptr++) {
		c = *ptr;
		if (!std::isalnum(c) and c != '.' and c != '-' and c != '_') break;
	}
	if (i < sizeof("n.xxx" - 1)) return false;
	std::string tmp = buf.substr(0, i);
	std::string end = tmp.substr(tmp.size() - (sizeof(".xxx") - 1));
	if (end.front() != '.') return false;
	for (size_t j = 1; j < end.size(); j++)
		if (!std::isalnum(end.at(j))) return false;
	filename = tmp;
	buf.erase(0, i);
	return true;
}

bool parseFileSize(std::string &buf, int *file_size) {
	if (!buf.size()) return false;
	size_t i = 0;
	char *ptr = buf.data(), c;
	while ((c = *ptr) != '\0') {
		if (!std::isdigit(c)) break;
		i++;
		ptr++;
	}
	char *end_ptr{};
	errno = 0;
	long tmp = std::strtol(buf.substr(0, i).c_str(), &end_ptr, 10);
	if (tmp == 0)
		return false;
	else if (errno == ERANGE or tmp < 0 or tmp > MAX_FILESIZE)
		return false;
	*file_size = (int)tmp;
	buf.erase(0, i);
	return true;
}
