#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <cstddef>

extern const char *GSIP;
extern const char *GSPORT;

extern size_t PLID_LEN;
extern size_t TIME_LEN;
extern size_t CODE_LEN;

extern int MAX_TIME;

enum Colors {
	RED = 'R',
	GREEN = 'G',
	BLUE = 'B',
	YELLOW = 'Y',
	ORANGE = 'O',
	PURPLE = 'P'
};

#endif
