#ifndef AUX_H
#define AUX_H

#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#define GAMES_DIR "games/"
#define FINISHEDGAME_DIR "finish_games/"
#define SCORES_DIR "scores/"

#include "../common/common.hpp"

class Player {
 private:
	std::string plid;
	bool game = false;
	int time = 0;
	std::vector<Color> code;
	int trials = 0;

 public:
	Player(std::string s);
	std::string getPlid();
	bool getGame();
	int getTime();
	void startGame(int t);
	void startGame(int t, std::vector<Color> c);
	std::vector<Color> getCode();
	int getTrials();
	void incrementTrials();
	int compareTrials(int t);
	void endGame();
};

class GameFile {
 public:
	static std::string createGameFileName(const std::string& plid);

	static bool saveGameStart(const std::string& plid, char mode,
														const std::string& code, int time);

	static bool saveGameTrial(const std::string& plid, const std::string& guess,
														int correct, int wrong);

	static bool checkTrial(const std::string& plid, const std::string& guess);

	static bool checkTimeExceeded(const std::string& plid);

	static bool finishGame(const std::string& plid, char outcome);

 private:
	static std::chrono::system_clock::time_point getGameStartTime(
			const std::string& plid);
	static bool setupDirectories();
	static int getGameTime(const std::string& plid);
};

enum Command { CMD_ERR, CMD_SNG, CMD_TRY, CMD_QUT, CMD_DBG, CMD_STR, CMD_SSB };

enum SNGResponse { OK, NOK, ERR };

std::string getCmdName(Command cmd);

#endif