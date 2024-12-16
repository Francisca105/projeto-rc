#ifndef COMMON_H
#define COMMON_H

#define GSIP \
	"127.0.0.1"	 // TODO: Check if can use localhost or needs to get the ip
#define GSPORT "58050"
#define CODE_LEN 7
#define CODE_COLOR_LEN 4
#define TIME_LEN 3
#define PLID_LEN 6
#define MAX_TIME 600
#define MAX_TRIALS 8
#define NUM_COLORS 4

enum Color {
	RED = 'R',
	GREEN = 'G',
	BLUE = 'B',
	YELLOW = 'Y',
	ORANGE = 'O',
	PURPLE = 'P'
};

#endif
