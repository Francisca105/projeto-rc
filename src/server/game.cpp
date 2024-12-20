#include "game.hpp"

#include <time.h>

#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <set>
#include <sstream>
#include <unordered_map>

#include "protocol.hpp"

bool hasActiveGame(std::string plid) {
	return std::filesystem::exists("./server/game_" + plid + ".txt");
}

bool hasFinishedGame(std::string plid) {
	return !std::filesystem::is_empty("./server/games/" + plid + "/");
}

bool startGame(std::string plid, int max_time, std::string &code) {
	std::string mode = " D ";
	if (code.front() == '\0') {
		generateCode(code);
		mode = " P ";
	}

	time_t start;
	time(&start);
	struct tm *now;
	char time_str[72] = {'\0'};	 // fixed value so compiler doesn't complain
	now = gmtime(&start);
	sprintf(time_str, "%4d-%02d-%02d %02d:%02d:%02d", now->tm_year + 1900,
					now->tm_mon + 1, now->tm_mday, now->tm_hour, now->tm_min,
					now->tm_sec);

	std::ofstream file("./server/game_" + plid + ".txt");
	if (file.is_open()) {
		std::ostringstream oss;
		oss << plid << mode << code << " " << max_time << " " << time_str << " "
				<< start << "\n";
		file << oss.str();
		if (file.fail()) {
			std::cerr << "Error saving file" << std::endl;
			file.close();
			return false;
		}
		file.close();
	} else {
		std::cerr << "Error opening file" << std::endl;
		return false;
	}

	return true;
}

void generateCode(std::string &code) {
	char colors[] = {'R', 'G', 'B', 'Y', 'P', 'O'};
	std::srand((unsigned int)std::time(0));
	for (size_t i = 0; i < CODE_SIZE; i++) {
		code[i] = colors[rand() % NUM_COLORS];
	}
}

bool checkTimeout(std::string plid, std::string &code) {
	if (!hasActiveGame(plid)) return false;

	std::ifstream file("./server/game_" + plid + ".txt");
	if (file.is_open()) {
		std::string _, tmp;
		int max_time;
		long seconds;

		file >> _ >> _ >> tmp >> max_time >> _ >> _ >> seconds;
		code.resize(sizeof("C C C C") - 1);
		sprintf(code.data(), "%c %c %c %c", tmp[0], tmp[1], tmp[2], tmp[3]);

		if (file.fail()) {
			std::cerr << "Error reading file" << std::endl;
			file.close();
			return false;
		}

		file.close();

		time_t curr_time;
		time(&curr_time);
		if (std::difftime(curr_time, (time_t)seconds) >= max_time) {
			std::cout << "[LOG] " << plid << " - Max playtime exceeded" << std::endl;
			timeoutGame(plid);
			return true;
		}

	} else {
		std::cerr << "Error opening file" << std::endl;
		return false;
	}
	return false;
}

bool timeoutGame(std::string plid) {
	time_t end;
	time(&end);
	struct tm *now;
	char time_str[68] = {'\0'};		 // fixed value so compiler doesn't complain
	char time_str_f[72] = {'\0'};	 // fixed value so compiler doesn't complain
	now = gmtime(&end);
	sprintf(time_str, "%4d-%02d-%02d %02d:%02d:%02d", now->tm_year + 1900,
					now->tm_mon + 1, now->tm_mday, now->tm_hour, now->tm_min,
					now->tm_sec);
	sprintf(time_str_f, "%4d%02d%02d_%02d%02d%02d", now->tm_year + 1900,
					now->tm_mon + 1, now->tm_mday, now->tm_hour, now->tm_min,
					now->tm_sec);

	std::fstream file("./server/game_" + plid + ".txt");
	if (file.is_open()) {
		std::string _;
		int max_time;

		file >> _ >> _ >> _ >> max_time;

		if (file.fail()) {
			std::cerr << "Error reading file" << std::endl;
			file.close();
			return false;
		}

		std::ostringstream oss;
		oss << time_str << " " << max_time << "\n";
		file.seekp(0, std::ios_base::end);
		file << oss.str();

		if (file.fail()) {
			std::cerr << "Error writing file" << std::endl;
			file.close();
			return false;
		}

		file.close();

	} else {
		std::cerr << "Error opening file" << std::endl;
		return false;
	}

	std::filesystem::path p = std::filesystem::current_path();
	if (!std::filesystem::is_directory(p / "server/games" / plid)) {
		if (!std::filesystem::create_directory(p / "server/games" / plid)) {
			std::cerr << "Error creating directory" << std::endl;
			return false;
		}
	}

	std::string old_filename = "game_" + plid + ".txt";
	std::string new_filename = std::string(time_str_f) + "_T.txt";
	std::filesystem::rename(p / "server" / old_filename,
													p / "server/games" / plid / new_filename);
	std::filesystem::remove(old_filename);

	return true;
}

bool winGame(std::string plid, std::string code, char nT) {
	time_t end;
	time(&end);
	struct tm *now;
	char time_str[68] = {'\0'};		 // fixed value so compiler doesn't complain
	char time_str_f[72] = {'\0'};	 // fixed value so compiler doesn't complain
	now = gmtime(&end);
	sprintf(time_str, "%4d-%02d-%02d %02d:%02d:%02d", now->tm_year + 1900,
					now->tm_mon + 1, now->tm_mday, now->tm_hour, now->tm_min,
					now->tm_sec);
	sprintf(time_str_f, "%4d%02d%02d_%02d%02d%02d", now->tm_year + 1900,
					now->tm_mon + 1, now->tm_mday, now->tm_hour, now->tm_min,
					now->tm_sec);

	char mode;
	std::fstream file("./server/game_" + plid + ".txt");
	if (file.is_open()) {
		std::string _;
		int max_time;
		long start;

		file >> _ >> mode >> _ >> max_time >> _ >> _ >> start;

		if (file.fail()) {
			std::cerr << "Error reading file" << std::endl;
			file.close();
			return false;
		}

		double time = difftime((time_t)end, (time_t)start);
		std::ostringstream oss;
		oss << time_str << " " << time << "\n";
		file.seekp(0, std::ios_base::end);
		file << oss.str();

		if (file.fail()) {
			std::cerr << "Error writing file" << std::endl;
			file.close();
			return false;
		}

		file.close();

	} else {
		std::cerr << "Error opening file" << std::endl;
		return false;
	}

	std::filesystem::path p = std::filesystem::current_path();
	if (!std::filesystem::is_directory(p / "server/games" / plid)) {
		if (!std::filesystem::create_directory(p / "server/games" / plid)) {
			std::cerr << "Error creating directory" << std::endl;
			return false;
		}
	}
	saveScore(100 - 10 * (int)(nT - '0'), plid, time_str_f, code, nT, mode);

	std::string old_filename = "game_" + plid + ".txt";
	std::string new_filename = std::string(time_str_f) + "_W.txt";
	std::filesystem::rename(p / "server" / old_filename,
													p / "server/games" / plid / new_filename);
	std::filesystem::remove(old_filename);

	return true;
}

void saveScore(int score, std::string plid, char *time_str, std::string code,
							 char tries, char mode) {
	std::string score_str(3, '\0'), time_s(time_str);
	sprintf(score_str.data(), "%03d", score);
	std::string mode_str = mode == 'P' ? "PLAY" : "DEBUG";

	std::fstream file(
			"./server/scores/" + score_str + "_" + plid + "_" + time_s + ".txt",
			std::ios::out);
	if (file.is_open()) {
		std::ostringstream oss;
		oss << score_str << " " << plid << " " << code << " " << tries << " "
				<< mode_str << "\n";
		file.seekp(0, std::ios_base::end);
		file << oss.str();

		if (file.fail()) {
			std::cerr << "Error writing file" << std::endl;
			file.close();
			return;
		}

		file.close();

	} else {
		std::cerr << "Error opening file" << std::endl;
		return;
	}
}

bool failGame(std::string plid) {
	time_t end;
	time(&end);
	struct tm *now;
	char time_str[68] = {'\0'};		 // fixed value so compiler doesn't complain
	char time_str_f[72] = {'\0'};	 // fixed value so compiler doesn't complain
	now = gmtime(&end);
	sprintf(time_str, "%4d-%02d-%02d %02d:%02d:%02d", now->tm_year + 1900,
					now->tm_mon + 1, now->tm_mday, now->tm_hour, now->tm_min,
					now->tm_sec);
	sprintf(time_str_f, "%4d%02d%02d_%02d%02d%02d", now->tm_year + 1900,
					now->tm_mon + 1, now->tm_mday, now->tm_hour, now->tm_min,
					now->tm_sec);

	std::fstream file("./server/game_" + plid + ".txt");
	if (file.is_open()) {
		std::string _;
		int max_time;

		file >> _ >> _ >> _ >> max_time;

		if (file.fail()) {
			std::cerr << "Error reading file" << std::endl;
			file.close();
			return false;
		}

		std::ostringstream oss;
		oss << time_str << " " << max_time << "\n";
		file.seekp(0, std::ios_base::end);
		file << oss.str();

		if (file.fail()) {
			std::cerr << "Error writing file" << std::endl;
			file.close();
			return false;
		}

		file.close();

	} else {
		std::cerr << "Error opening file" << std::endl;
		return false;
	}

	std::filesystem::path p = std::filesystem::current_path();
	if (!std::filesystem::is_directory(p / "server/games" / plid)) {
		if (!std::filesystem::create_directory(p / "server/games" / plid)) {
			std::cerr << "Error creating directory" << std::endl;
			return false;
		}
	}

	std::string old_filename = "game_" + plid + ".txt";
	std::string new_filename = std::string(time_str_f) + "_F.txt";
	std::filesystem::rename(p / "server" / old_filename,
													p / "server/games" / plid / new_filename);
	std::filesystem::remove(old_filename);

	return true;
}

bool quitGame(std::string plid, std::string &key) {
	time_t end;
	time(&end);
	struct tm *now;
	char time_str[68] = {'\0'};		 // fixed value so compiler doesn't complain
	char time_str_f[72] = {'\0'};	 // fixed value so compiler doesn't complain
	now = gmtime(&end);
	sprintf(time_str, "%4d-%02d-%02d %02d:%02d:%02d", now->tm_year + 1900,
					now->tm_mon + 1, now->tm_mday, now->tm_hour, now->tm_min,
					now->tm_sec);
	sprintf(time_str_f, "%4d%02d%02d_%02d%02d%02d", now->tm_year + 1900,
					now->tm_mon + 1, now->tm_mday, now->tm_hour, now->tm_min,
					now->tm_sec);

	std::fstream file("./server/game_" + plid + ".txt");
	if (file.is_open()) {
		std::string _;
		int max_time;

		file >> _ >> _ >> _ >> max_time;

		if (file.fail()) {
			std::cerr << "Error reading file" << std::endl;
			file.close();
			return false;
		}

		sprintf(key.data(), "%c %c %c %c", _[0], _[1], _[2], _[3]);

		std::ostringstream oss;
		oss << time_str << " " << max_time << "\n";
		file.seekp(0, std::ios_base::end);
		file << oss.str();

		if (file.fail()) {
			std::cerr << "Error writing file" << std::endl;
			file.close();
			return false;
		}

		file.close();

	} else {
		std::cerr << "Error opening file" << std::endl;
		return false;
	}

	std::filesystem::path p = std::filesystem::current_path();
	if (!std::filesystem::is_directory(p / "server/games" / plid)) {
		if (!std::filesystem::create_directory(p / "server/games" / plid)) {
			std::cerr << "Error creating directory" << std::endl;
			return false;
		}
	}

	std::string old_filename = "game_" + plid + ".txt";
	std::string new_filename = std::string(time_str_f) + "_Q.txt";
	std::filesystem::rename(p / "server" / old_filename,
													p / "server/games" / plid / new_filename);
	std::filesystem::remove(old_filename);

	return true;
}

bool registerTry(std::string plid, std::string guess, int nB, int nW) {
	std::fstream file("./server/game_" + plid + ".txt");
	if (file.is_open()) {
		std::string _;
		long start;

		file >> _ >> _ >> _ >> _ >> _ >> _ >> start;

		if (file.fail()) {
			std::cerr << "Error saving file" << std::endl;
			file.close();
			return false;
		}

		time_t now;
		time(&now);
		double s = difftime(now, (time_t)start);

		std::ostringstream oss;
		oss << "T: " << guess << " " << nB << " " << nW << " " << s << "\n";
		file.seekp(0, std::ios_base::end);
		file << oss.str();
		if (file.fail()) {
			std::cerr << "Error saving file" << std::endl;
			file.close();
			return false;
		}
		file.close();
	} else {
		std::cerr << "[DEBUG] Error opening file" << std::endl;
		return false;
	}

	return true;
}

Rtr checkTry(std::string plid, std::string code, int nT) {
	std::string key;
	std::vector<std::string> guesses;

	getTrials(plid, key, guesses);

	if (guesses.size() and isInv(code, guesses.back(), nT, (int)guesses.size()))
		return INV;
	if (isDup(code, guesses)) return DUP;
	if (isEnt(code, key, nT)) return ENT;
	if (nT == (int)guesses.size() and code.compare(guesses.back())) return Ok;
	return OK;
}

bool isInv(std::string guess, std::string last, int nT, int tries) {
	if (nT == tries and guess.compare(last)) return true;
	if (nT < tries or nT > tries + 1) return true;
	return false;
}

bool isDup(std::string guess, std::vector<std::string> guesses) {
	for (size_t i = 0; i < guesses.size(); i++) {
		if (!guesses.at(i).compare(guess)) return true;
	}
	return false;
}

bool isEnt(std::string guess, std::string key, int nT) {
	if (guess.compare(key) and nT == MAX_TRIES) return true;
	return false;
}

void getTrials(std::string plid, std::string &key,
							 std::vector<std::string> &guesses) {
	std::ifstream file("./server/game_" + plid + ".txt");
	if (file.is_open()) {
		std::string line;
		std::getline(file, line);
		key = line.substr(sizeof("PPPPPP M ") - 1, CODE_SIZE);

		bool good = false;
		while (file) {
			std::getline(file, line);
			if (file.eof()) {
				good = true;
				break;
			}

			std::istringstream iss(line);
			std::string _;
			iss >> _ >> _;
			guesses.push_back(_);
		}

		if (!good and file.fail()) {
			std::cerr << "Error reading file" << std::endl;
			file.close();
			return;
		}

		file.close();
	} else {
		std::cerr << "Error opening file" << std::endl;
		return;
	}
}

std::string getKey(std::string plid) {
	std::string key, _;
	std::ifstream file("./server/game_" + plid + ".txt");
	if (file.is_open()) {
		file >> _ >> _ >> key;

		if (file.fail()) {
			std::cerr << "Error reading file" << std::endl;
			file.close();
			return std::string();
		}

		file.close();
	} else {
		std::cerr << "Error opening file" << std::endl;
		return std::string();
	}
	return key;
}

void getHints(std::string guess, std::string key, int *nB, int *nW) {
	*nB = 0;
	*nW = 0;
	key.erase(1, 1).erase(2, 1).erase(3, 1).erase(4, 1);

	std::unordered_map<char, int> remainingChars;
	for (size_t i = 0; i < guess.size(); ++i) {
		if (guess[i] == key[i]) {
			(*nB)++;
		} else {
			remainingChars[key[i]]++;
		}
	}

	for (size_t i = 0; i < guess.size(); ++i) {
		if (guess[i] != key[i] && remainingChars[guess[i]] > 0) {
			(*nW)++;
			remainingChars[guess[i]]--;
		}
	}
}

void sendScoreboard(int fd, int *n_sb) {
	std::vector<std::filesystem::directory_entry> entries;

	for (const auto &entry :
			 std::filesystem::directory_iterator("./server/scores/")) {
		entries.push_back(entry);
	}

	std::sort(entries.begin(), entries.end(),
						[](const std::filesystem::directory_entry &a,
							 const std::filesystem::directory_entry &b) {
							return a.path().filename() > b.path().filename();
						});

	std::string score = "", plid = "", code = "", trials = "", mode = "";
	std::ostringstream scoreboard;

	for (size_t i = 0; i < std::min(entries.size(), (size_t)SCOREBOARD_LEN);
			 i++) {
		std::fstream file(entries[i].path(), std::ios::in);
		if (file.is_open()) {
			file >> score >> plid >> code >> trials >> mode;
			if (file.fail()) {
				std::cerr << "Error reading file" << std::endl;
				file.close();
				return;
			}
			scoreboard << i + 1 << " - " << score << " " << plid << " " << code << " "
								 << trials << " " << mode << '\n';

			file.close();
		} else {
			std::cerr << "Error opening file" << std::endl;
			return;
		}
	}

	std::string fdata = scoreboard.str();
	size_t fsize = fdata.size();
	std::string packet = "RSS OK topscores_" + std::to_string(*n_sb++) + ".txt";
	packet += " " + std::to_string(fsize) + " " + fdata + "\n";
	sendTcp(packet, fd);
}

void sendActiveGame(std::string plid, int fd) {
	int max_time;
	long start;
	std::string _, fdata;
	std::fstream file("./server/game_" + plid + ".txt");
	if (file.is_open()) {
		file >> _ >> _ >> _ >> max_time >> _ >> _ >> start;
		bool good = false;
		while (file) {
			std::getline(file, _);
			if (file.eof()) {
				good = true;
				break;
			}
			fdata.append(_ + "\n");
		}
		if (!good and file.fail()) {
			std::cerr << "Error reading file" << std::endl;
			file.close();
			return;
		}

		file.close();
	} else {
		std::cerr << "Error opening file" << std::endl;
		return;
	}

	time_t now;
	time(&now);
	int remaining = (int)((double)max_time - difftime(now, (time_t)start));
	std::cout << "1:" << fdata;
	fdata.erase(0, 1);
	std::cout << "2:" << fdata;
	fdata.append(std::to_string(remaining) + "\n");
	std::cout << "3:" << fdata;
	size_t fsize = fdata.size();

	std::string packet = "RST ACT trials_" + plid + ".txt " +
											 std::to_string(fsize) + " " + fdata + "\n";
	sendTcp(packet, fd);
}

void sendLastGame(std::string plid, int fd) {
	std::string fname;
	if (!FindLastGame(plid, fname)) {
		std::cerr << "Error opening file" << std::endl;
	}

	std::stringstream content;
	std::ifstream file(fname);

	if (file.is_open()) {
		content << file.rdbuf();
		file.close();

		std::string data = content.str();
		std::string packet = "RST FIN trials" + plid + ".txt " +
												 std::to_string(data.size()) + " " + data + "\n";
		sendTcp(packet, fd);
	} else {
		std::cerr << "Error opening file" << std::endl;
		return;
	}
}

int FindLastGame(std::string PLID, std::string &fname) {
	std::filesystem::path dirname = "./server/games/" + PLID;
	int found = 0;

	if (!std::filesystem::exists(dirname)) {
		return 0;
	}

	std::vector<std::filesystem::path> files;
	for (const auto &entry : std::filesystem::directory_iterator(dirname)) {
		if (entry.path().filename().string()[0] != '.') {
			files.push_back(entry.path());
		}
	}

	if (!files.empty()) {
		std::sort(files.begin(), files.end());
		fname = files.back().string();
		found = 1;
	}

	return found;
}
