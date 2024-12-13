#include "client_game.hpp"

void runCmd(Command cmd, ClientArgs args, ClientState *state) {
	if (args.time == 0 and state->fd == -1) return;
	switch (cmd) {
		case CMD_START:
			// runStart(args, state);
			break;
		case CMD_TRY:
			// runTry(args, state);
			break;
		case CMD_QUIT:
			// runQuit(args, state);
			break;
		case CMD_EXIT:
			// runExit(args, state);
			break;
		case CMD_DEBUG:
			// runDebug(args, state);
			break;
		case CMD_SHOWTRIALS:
			// runShowTrials(args, state);
			break;
		case CMD_SCOREBOARD:
			// runScoreboard(args, state);
			break;
		case CMD_ERR:
		default:
			break;
	}
}

// void runStart(ClientArgs args, ClientState *state) {

// }

// void runTry(ClientArgs args, ClientState *state) {

// }

// void runQuit(ClientArgs args, ClientState *state) {

// }

// void runExit(ClientArgs args, ClientState *state) {

// }

// void runDebug(ClientArgs args, ClientState *state) {

// }

// void runShowTrials(ClientArgs args, ClientState *state) {

// }

// void runScoreboard(ClientArgs args, ClientState *state) {

// }
