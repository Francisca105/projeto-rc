#include "support.hpp"

volatile sig_atomic_t KeepRunning = true;

std::unordered_map<std::string, Status> StatusDict = {
		{"OK", OK},		{"NOK", NOK}, {"ERR", ERR}, {"DUP", DUP}, {"INV", INV},
		{"ENT", ENT}, {"ETM", ETM}, {"ACT", ACT}, {"FIN", FIN}, {"EMPTY", EMPTY}};
