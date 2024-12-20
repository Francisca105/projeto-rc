#ifndef SUPPORT_H
#define SUPPORT_H

#include <time.h>

#include <string>

#define TCP_CONNECTIONS 10
#define SERVER_TIMEOUT 30

#define MAX_REQUEST_LEN 24

typedef struct {
	int time_limit;

	std::string plid;
} ActiveGame;

typedef struct {
} FinishedGame;

// extern volatile __sig_atomic_t KeepRunning;

#endif
