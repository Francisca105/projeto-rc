#include "aux.hpp"

Player::Player(std::string s) {
	plid = s;
};

std::string Player::getPlid() {
	return plid;
}

bool Player::getGame() {
	return game;
}

int Player::getTime() {
	return time;
}

void Player::startGame(int t) {
	game = true;
	time = t;
}

void Player::endGame() {
	game = false;
	time = 0;
}

std::string getCmdName(Command cmd) {
	switch (cmd) {
		case CMD_ERR:
			return std::string("ERR");
		case CMD_SNG:
			return std::string("SNG");
		case CMD_TRY:
			return std::string("TRY");
		case CMD_QUT:
			return std::string("QUT");
		case CMD_DBG:
			return std::string("DBG");
		case CMD_STR:
			return std::string("STR");
		case CMD_SSB:
			return std::string("SSB");
		default:
			return std::string("ERR");
	}
}
