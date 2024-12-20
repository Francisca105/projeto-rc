#ifndef MAIN_H
#define MAIN_H

#include <signal.h>

extern volatile sig_atomic_t KeepRunning;

#define SERVER_TIMEOUT 30

void sigintHandler(int signal);

#endif
