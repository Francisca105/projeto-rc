#include "aux.hpp"

#include "game.hpp"
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

std::vector<Color> Player::getCode() {
	return code;
}

int Player::getTrials() {
	return trials;
}

void Player::incrementTrials() {
	trials++;
}

int Player::compareTrials(int t) {
	return t == trials;
}

void Player::startGame(int t) {
	game = true;
	time = t;
	code = GameUtils::generateCode();
	trials = 0;
}

void Player::startGame(int t, std::vector<Color> c) {
	game = true;
	time = t;
	code = c;
	trials = 0;
}

void Player::endGame() {
	game = false;
	time = 0;
	trials = 0;
	code.clear();
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
