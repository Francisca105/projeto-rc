#ifndef GAME_H
#define GAME_H

#include <time.h>

#include <string>
#include <vector>

#define CODE_SIZE 4
#define NUM_COLORS 6
#define MAX_TRIES 8

#define SCOREBOARD_LEN 10

enum Rtr { OK, Ok, DUP, INV, ENT };

bool hasActiveGame(std::string plid);
bool hasFinishedGame(std::string plid);
bool startGame(std::string plid, int max_time, std::string &code);
void generateCode(std::string &code);
bool checkTimeout(std::string plid, std::string &code);
bool timeoutGame(std::string plid);
Rtr checkTry(std::string plid, std::string code, int nT);
void getTrials(std::string plid, std::string &key,
							 std::vector<std::string> &guesses);
bool isDup(std::string guess, std::vector<std::string> guesses);
bool isInv(std::string guess, std::string last, int nT, int tries);
bool isEnt(std::string guess, std::string key, int nT);
std::string getKey(std::string plid);
void getHints(std::string guess, std::string key, int *nB, int *nW);
bool registerTry(std::string plid, std::string guess, int nB, int nW);
bool winGame(std::string plid, std::string code, char nT);
bool failGame(std::string plid);
bool quitGame(std::string plid, std::string &key);
void saveScore(int score, std::string plid, char *time_str, std::string code,
							 char tries, char mode);
int FindLastGame(std::string PLID, std::string &fname);
void sendScoreboard(int fd, int *n_sb);
void sendActiveGame(std::string plid, int fd);
void sendLastGame(std::string plid, int fd);

#endif
