#ifndef AUX_H
#define AUX_H

#include <iostream>

class Player {
 private:
	std::string plid;
	bool game = false;
	int time = 0;

 public:
	Player(std::string s);
	std::string getPlid();
	bool getGame();
	int getTime();
	void startGame(int t);
	void endGame();
};

enum Command { CMD_ERR, CMD_SNG, CMD_TRY, CMD_QUT, CMD_DBG, CMD_STR, CMD_SSB };

enum SNGResponse { OK, NOK, ERR };

std::string getCmdName(Command cmd);

#endif