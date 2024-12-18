#ifndef SUPPORT_H
#define SUPPORT_H

#include <netdb.h>
#include <signal.h>

#include <string>
#include <vector>

#define GSIP "localhost"
#define GSPORT "58050"

#define CMD_LEN 3
#define CODE_LEN 7
#define PLID_LEN 6
#define TIME_LEN 3

#define MAX_TIME 600
#define NUM_COLORS 4
#define MAX_TRIES 8

#define UDP_TIMEOUT 10
#define UDP_MAX_TRIES 3
#define TCP_TIMEOUT 10

#define MAX_FILENAME 24
#define MAX_FILESIZE 2048
#define FILES_DIR "./gamedata/"

// Client/Server commands
enum Cmd { Start, Try, Quit, Exit, Debug, Showtrials, Scoreboard, Invalid };

// Server status reply
enum Status { OK, NOK, ERR, DUP, INV, ENT, ETM, ACT, FIN, EMPTY };

// Client config
typedef struct {
	std::string ip = GSIP;
	std::string port = GSPORT;
	int udp_fd;
	struct addrinfo *server_addr;
} Config;

// Client state
typedef struct {
	bool playing = false;
	std::string plid;
	int nT = 0;
} State;

// Client args
typedef struct {
	std::string plid;
	int time = 0;
	std::string code;
} ClArgs;

// ShowTrials server args
typedef struct {
	std::string fname;
	int fsize = 0;
	std::string data;
} Trials;

typedef struct {
	std::string fname;
	int fsize = 0;
	std::string data;
} ScoreBoard;

// Server args
typedef struct {
	Status status;
	int nT = 0;
	int nB = -1;
	int nW = -1;
	std::string code;
	Trials trials;
	ScoreBoard scoreboard;
} SvArgs;

// It is used to close the application when Ctrl-C is pressed
extern volatile sig_atomic_t KeepRunning;

#endif
