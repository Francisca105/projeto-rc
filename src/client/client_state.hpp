#ifndef CLIENT_STATE_H
#define CLIENT_STATE_H
#include <iostream>

#include "../common/common.hpp"

class ClientState {
	std::string gs_ip = GSIP;
	std::string gs_port = GSPORT;
	std::string plid = "";
	bool keep_running = true;

 public:
	ClientState(int argc, char **argv);
	void setIp(std::string ip);
	void setPort(std::string port);
	void setPlid(std::string plid);
	std::string getPlid();
};

void initState(int argc, char **argv, ClientState &state);

#endif
