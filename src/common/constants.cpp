#include "constants.hpp"

#include <cstdint>
#include <string>
#include <unordered_map>

const char *GSIP = "localhost";
const char *GSPORT = "58050";

size_t PLID_LEN = 6;
size_t TIME_LEN = 3;
size_t CODE_LEN = 7;

int MAX_TIME = 600;

size_t SNG_LEN = std::string("SNG PPPPPP TTT\n").size();
size_t TRY_LEN = std::string("TRY PPPPPP C C C C N\n").size();
size_t QUT_LEN = std::string("QUT PPPPPP\n").size();
size_t DBG_LEN = std::string("DBG PPPPPP TTT C C C C\n").size();
size_t STR_LEN = std::string("STR PPPPPP\n").size();
size_t SSB_LEN = std::string("SSB PPPPPP\n").size();

size_t MAX_UDP_REPLY = std::string("RTR SSS C C C C\n").size();

int UDP_TIMEOUT = 10;

std::uint32_t MAX_UDP_TRIES = 3;

size_t CMD_LEN = 3;

std::unordered_map<std::string, Status> Temp = {
		{"OK", OK},		{"NOK", NOK}, {"ERR", ERR}, {"DUP", DUP},
		{"INV", INV}, {"ENT", ENT}, {"ETM", ETM}};
