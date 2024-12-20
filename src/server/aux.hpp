#ifndef AUX_H
#define AUX_H

#include <iostream>
#include <vector>

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

enum Command { CMD_ERR, CMD_SNG, CMD_TRY, CMD_QUT, CMD_DBG, CMD_STR, CMD_SSB };

enum SNGResponse { OK, NOK, ERR };

std::string getCmdName(Command cmd);

#endif