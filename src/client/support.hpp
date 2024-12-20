#ifndef SUPPORT_H
#define SUPPORT_H

#include <netdb.h>
#include <signal.h>

#include <string>
#include <vector>

#define MAX_FILESIZE 2048

// Server status reply
enum Status { OK, NOK, ERR, DUP, INV, ENT, ETM, ACT, FIN, EMPTY };

// It is used to close the application when Ctrl-C is pressed
extern volatile sig_atomic_t KeepRunning;

#endif
