#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <cstddef>
#include <cstdint>
#include <string>
#include <unordered_map>

extern const char *GSIP;
extern const char *GSPORT;

extern size_t PLID_LEN;
extern size_t TIME_LEN;
extern size_t CODE_LEN;

extern int MAX_TIME;

extern size_t SNG_LEN;
extern size_t TRY_LEN;
extern size_t QUT_LEN;
extern size_t DBG_LEN;
extern size_t STR_LEN;
extern size_t SSB_LEN;

extern size_t MAX_UDP_REPLY;

extern int UDP_TIMEOUT;

extern std::uint32_t MAX_UDP_TRIES;

extern size_t CMD_LEN;

enum Colors {
	RED = 'R',
	GREEN = 'G',
	BLUE = 'B',
	YELLOW = 'Y',
	ORANGE = 'O',
	PURPLE = 'P'
};

enum Status { OK, NOK, ERR, DUP, INV, ENT, ETM };

extern std::unordered_map<std::string, Status> Temp;

#endif
