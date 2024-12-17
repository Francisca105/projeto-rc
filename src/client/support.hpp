#ifndef SUPPORT_H
#define SUPPORT_H

#include <netdb.h>
#include <signal.h>

#include <string>
#include <unordered_map>

#define GSIP "localhost"
#define GSPORT "58050"

#define CMD_LEN 3
#define PLID_LEN 6
#define MAX_TIME 600
#define CODE_LEN 7
#define NUM_COLORS 4

#define MAX_TRIES 8

#define FILE_NAME_MAX 24
#define FILESIZE_MAX 2048

typedef struct {
	std::string ip = GSIP;
	std::string port = GSPORT;
	int udp_fd;
	struct addrinfo *server_addr;
	bool playing = false;
	std::string plid;
	int nT;
} ClientState;

typedef struct {
	std::string plid;
	int time;
	std::string code;
	std::string file_name;
	int file_size;
	std::string file_data;
} ClientArgs;

enum Status { OK, NOK, ERR, DUP, INV, ENT, ETM, ACT, FIN, EMPTY };

typedef struct {
	Status status;
	int nT;
	int nB;
	int nW;
	std::string code;
	std::string fname;
	int fsize;
	std::string fdata;
} ServerArgs;

enum Command {
	CMD_START,
	CMD_TRY,
	CMD_QUIT,
	CMD_EXIT,
	CMD_DEBUG,
	CMD_SHOWTRIALS,
	CMD_SCOREBOARD,
	CMD_INV
};

enum Colors {
	RED = 'R',
	GREEN = 'G',
	BLUE = 'B',
	YELLOW = 'Y',
	ORANGE = 'O',
	PURPLE = 'P'
};

enum Packet { GOOD, NOT_SENT, NOT_REPLIED, INTERRUPT };

extern volatile sig_atomic_t KeepRunning;

extern std::unordered_map<std::string, Status> StatusDict;

#endif
