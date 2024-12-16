#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <unordered_map>

#include "aux.hpp"

#define UDP true
#define TCP false
#define MAX_CONNECTIONS 10
#define UDP_SIZE 23	 // TODO: check what size should it be / better naming
#define TCP_SIZE 11	 // TODO: check what size should it be / better naming

typedef struct client Client;
typedef struct parameters Parameters;

int runServer(std::string port_number, bool verbose);
int socketInnit(std::string port_number, bool protocol);
int readUdp(int fd, char *buf, char *host, char *service);
void parseAndRun(Command cmd, std::string buf, int len, Client client,
								 Parameters *params, bool verbose,
								 std::unordered_map<std::string, Player> &players);

void run_rsg(Parameters *params,
						 std::unordered_map<std::string, Player> &players);
void run_try(Parameters *params,
						 std::unordered_map<std::string, Player> &players);

void printVerbose(bool parsing, std::string plid, Command cmd, Client client);

void errorErrno(const char *func);
void errorGai(int errcode);

#endif