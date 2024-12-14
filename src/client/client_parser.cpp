#include "client_parser.hpp"

#include <algorithm>
#include <iostream>

#include "../common/constants.hpp"

Command getCmd(std::string buf) {
	std::string first = buf.substr(0, buf.find(' '));
	switch (first.front()) {
		case 's':
			if (!first.compare("start"))
				return CMD_START;
			else if (!first.compare("show_trials") or !first.compare("st"))
				return CMD_SHOWTRIALS;
			else if (!first.compare("scoreboard") or !first.compare("sb"))
				return CMD_SCOREBOARD;
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

bool parseCmd(std::string &buf, ClientArgs *args, Command cmd) {
	std::string first = buf.substr(0, buf.find(' '));
	std::string s = buf.size() == first.size() ? "" : buf.substr(buf.find(' '));
	bool error = true;
	switch (cmd) {
		case CMD_START:
			if (parseStart(s, args)) error = false;
			break;
		case CMD_TRY:
			if (parseTry(s, args)) error = false;
			break;
		case CMD_DEBUG:
			if (parseDebug(s, args)) error = false;
			break;
		case CMD_QUIT:
		case CMD_EXIT:
		case CMD_SHOWTRIALS:
		case CMD_SCOREBOARD:
			rtrim(s);
			if (!s.size()) error = false;
		case CMD_ERR:
		default:
			break;
	}
	if (error) buf = first;
	return !error;
}

bool parseStart(std::string buf, ClientArgs *args) {
	if (!parseSpace(buf)) return false;
	if (!parsePlid(buf, args->plid)) return false;
	if (!parseSpace(buf)) return false;
	if (!parseTime(buf, &args->time)) return false;
	rtrim(buf);
	if (buf.size() != 0) return false;
	return true;
}

bool parseTry(std::string buf, ClientArgs *args) {
	if (!parseSpace(buf)) return false;
	if (!parseCode(buf, args->code)) return false;
	rtrim(buf);
	if (buf.size() != 0) return false;
	return true;
}

bool parseDebug(std::string buf, ClientArgs *args) {
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

bool parsePlid(std::string &buf, std::string &plid) {
	if (!buf.size()) return false;
	size_t i = 0;
	char *ptr = buf.data(), c;
	while ((c = *ptr) != '\0') {
		if (!std::isdigit(c)) break;
		i++;
		ptr++;
	}
	if (i != PLID_LEN) {
		// std::cerr << "Error parsing PLID.\n";
		return false;
	}
	plid = buf.substr(0, PLID_LEN);	 //.data();
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
	if (tmp == 0) {
		// std::cerr << "max_playtime not a valid number.\n";
		return false;
	} else if (errno == ERANGE or tmp < 0 or tmp > MAX_TIME) {
		// std::cerr << "max_playtime must be between 1 and 600.\n";
		return false;
	}
	*time = (int)tmp;
	buf.erase(0, i);
	return true;
}

bool parseCode(std::string &buf, std::string &code) {
	if (buf.size() < CODE_LEN) return false;
	std::string tmp = buf;
	char *ptr = tmp.data(), c;
	for (int i = 0; i < 4; i++) {
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
				// std::cerr << "Invalid color.\n";
				return false;
		}
		tmp.erase(0, 1);
		if (i < 3 and !parseSpace(tmp)) return false;
	}
	code = buf.substr(0, CODE_LEN);
	buf.erase(0, CODE_LEN);
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

void rtrim(std::string &buf) {
	buf.erase(std::find_if(buf.rbegin(), buf.rend(),
												 [](unsigned char ch) { return !std::isspace(ch); })
								.base(),
						buf.end());
}

bool parseRSG(std::string buf, ServerArgs *args) {
	if (buf.size() < CMD_LEN or buf.substr(0, CMD_LEN).compare("RSG"))
		return false;
	buf.erase(0, CMD_LEN);
	if (!parseSpace(buf)) return false;
	if (!parseStatus(buf, &args->status)) return false;
	if (!parseNewline(buf)) return false;
	if (buf.size() != 0) return false;
	return true;
}

bool parseStatus(std::string &buf, Status *status) {
	char *ptr = buf.data();
	size_t i;
	for (i = 0; i < buf.size(); i++, ptr++) {
		if (!isupper(*ptr)) break;
	}
	if (auto it = Temp.find(buf.substr(0, i)); it != Temp.end()) {
		*status = it->second;
		buf.erase(0, i);
		return true;
	}
	return false;
}

bool parseRTR(std::string buf, ServerArgs *args) {
	if (buf.size() < CMD_LEN or buf.substr(0, CMD_LEN).compare("RTR"))
		return false;
	buf.erase(0, CMD_LEN);
	if (!parseSpace(buf)) return false;
	if (!parseStatus(buf, &args->status)) return false;
	switch (args->status) {
		case OK:
			if (!parseSpace(buf)) return false;
			if (!parseXXX(buf, &args->nT, &args->nB, &args->nW)) return false;
			args->nT += 1;
			break;
		case DUP:
			if (!parseSpace(buf)) return false;
			if (!parseXXX(buf, &args->nT, &args->nB, &args->nW)) return false;
			break;
		case ENT:
			if (!parseSpace(buf)) return false;
			if (!parseCode(buf, args->code)) return false;
			break;
		case ETM:
			if (!parseSpace(buf)) return false;
			if (!parseCode(buf, args->code)) return false;
			break;
		case INV:
			break;
		case NOK:
			break;
		case ERR:
		default:
			break;
	}
	if (!parseNewline(buf)) return false;
	if (buf.size() != 0) return false;
	return true;
}

bool parseXXX(std::string &buf, int *nT, int *nB, int *nW) {
	if (buf.size() < sizeof("T B W")) return false;
	long nT_l = std::atol(buf.substr(0, 1).c_str());
	if (nT_l <= 0 or nT_l > 7) return false;
	buf.erase(0, 1);
	if (!parseSpace(buf)) return false;
	long nB_l = std::atol(buf.substr(0, 1).c_str());
	if (nB_l == 0 and !std::isdigit(buf.substr(0, 1).c_str()[0])) return false;
	if (nB_l < 0 or nB_l > 3) return false;
	buf.erase(0, 1);
	if (!parseSpace(buf)) return false;
	long nW_l = std::atol(buf.substr(0, 1).c_str());
	if (nW_l == 0 and !std::isdigit(buf.substr(0, 1).c_str()[0])) return false;
	if (nW_l < 0 or nW_l > 4) return false;
	buf.erase(0, 1);
	*nT = (int)nT_l;
	*nB = (int)nB_l;
	*nW = (int)nW_l;
	return true;
}

bool parseQUT(std::string buf, ServerArgs *args) {
	if (buf.size() < CMD_LEN or buf.substr(0, CMD_LEN).compare("RQT"))
		return false;
	buf.erase(0, CMD_LEN);
	if (!parseSpace(buf)) return false;
	if (!parseStatus(buf, &args->status)) return false;
	switch (args->status) {
		case OK:
			if (!parseSpace(buf)) return false;
			if (!parseCode(buf, args->code)) return false;
			break;
		case NOK:
		case ERR:
			break;
		case DUP:
		case INV:
		case ENT:
		case ETM:
		default:
			return false;
	}
	if (!parseNewline(buf)) return false;
	if (buf.size() != 0) return false;
	return true;
}

bool parseDBG(std::string buf, ServerArgs *args) {
	if (buf.size() < CMD_LEN or buf.substr(0, CMD_LEN).compare("RDB"))
		return false;
	buf.erase(0, CMD_LEN);
	if (!parseSpace(buf)) return false;
	if (!parseStatus(buf, &args->status)) return false;
	if (!parseNewline(buf)) return false;
	if (buf.size() != 0) return false;
	return true;
}
