#ifndef GAME_H
#define GAME_H

#include <time.h>

#include <string>
#include <vector>

#define GAMES_PATH "/server/games/"

#define CODE_SIZE 4
#define NUM_COLORS 6
#define MAX_TRIES 8

typedef struct {
	int max_time;
	time_t start;
	int nT = 0;
	char code[4];
} Active;

typedef struct {
	time_t start;
	int max_time;
	bool mode;
	char code[4];
	std::vector<std::string> trials;
	time_t finish;
	int status;
} Finished;

enum Rtr {OK, Ok, DUP, INV, ENT};

bool hasActiveGame(std::string plid);
bool startGame(std::string plid, int max_time, std::string &code);
void generateCode(std::string &code);
bool checkTimeout(std::string plid, std::string &code);
bool timeoutGame(std::string plid);
Rtr checkTry(std::string plid, std::string code, int nT);
void getTrials(std::string plid, std::string &key, std::vector<std::string> &guesses);
bool isDup(std::string guess, std::vector<std::string> guesses);
bool isInv(std::string guess, std::string last, int nT, int tries);
bool isEnt(std::string guess, std::string key, int nT);
std::string getKey(std::string plid);
void aOk(std::string guess, std::string key, int *nB, int *nW);
bool registerTry(std::string plid, std::string guess, int nB, int nW);
bool winGame(std::string plid, std::string code, char nT);
bool failGame(std::string plid);
bool quitGame(std::string plid, std::string &key);
void saveScore(int score, std::string plid, char *time_str, std::string code, char tries, char mode);

#endif
