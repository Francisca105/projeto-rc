#include "game.hpp"

#include <cstdlib>
#include <ctime>
#include <string>
#include <vector>

namespace GameUtils {
// Verifications

int isColorValid(char c) {
	return c == RED || c == GREEN || c == BLUE || c == YELLOW || c == ORANGE ||
				 c == PURPLE;
}

int isGuessValid(char c1, char c2, char c3, char c4) {
	return isColorValid(c1) && isColorValid(c2) && isColorValid(c3) &&
				 isColorValid(c4);
}

int isTrialsExceeded(int trials) {
	return trials >= MAX_TRIALS;
}

// Auxiliary functions

char colorToChar(Color color) {
	return static_cast<char>(color);
}

ColorCount countColors(std::vector<Color> colors) {
	ColorCount colorCount = {0, 0, 0, 0, 0, 0};

	for (Color color : colors) {
		switch (color) {
			case RED:
				colorCount.red++;
				break;
			case GREEN:
				colorCount.green++;
				break;
			case BLUE:
				colorCount.blue++;
				break;
			case YELLOW:
				colorCount.yellow++;
				break;
			case ORANGE:
				colorCount.orange++;
				break;
			case PURPLE:
				colorCount.purple++;
				break;
			default:
				break;
		}
	}

	return colorCount;
}

std::vector<Color> charsToColors(char c1, char c2, char c3, char c4) {
	std::vector<Color> colors;
	colors.push_back(static_cast<Color>(c1));
	colors.push_back(static_cast<Color>(c2));
	colors.push_back(static_cast<Color>(c3));
	colors.push_back(static_cast<Color>(c4));

	return colors;
}

std::string colorsToString(std::vector<Color> colors) {
	std::string str;
	for (Color color : colors) {
		str.push_back(colorToChar(color));
	}
	return str;
}

std::string colorsToStringWithSpaces(std::vector<Color> colors) {
	std::string str;
	for (Color color : colors) {
		str.push_back(colorToChar(color));
		str.push_back(' ');
	}
	str.pop_back();	 // Remove last space
	return str;
}

// Game logic

std::vector<Color> generateCode() {
	std::vector<Color> colors;
	std::srand((uint)std::time(nullptr));	 // Seed for random number generation

	for (int i = 0; i < NUM_COLORS; ++i) {
		int randomIndex = std::rand() % 6;	// There are 6 colors
		switch (randomIndex) {
			case 0:
				colors.push_back(RED);
				break;
			case 1:
				colors.push_back(GREEN);
				break;
			case 2:
				colors.push_back(BLUE);
				break;
			case 3:
				colors.push_back(YELLOW);
				break;
			case 4:
				colors.push_back(ORANGE);
				break;
			case 5:
				colors.push_back(PURPLE);
				break;
			default:
				break;
		}
	}

	return colors;
}

TrialResult checkGuess(std::vector<Color> secret, std::vector<Color> guess) {
	TrialResult result = {0, 0};

	ColorCount secretCount = countColors(secret);
	ColorCount guessCount = countColors(guess);

	for (size_t i = 0; i < NUM_COLORS; ++i) {
		if (secret[i] == guess[i]) {
			result.correctColorAndPosition++;
			switch (secret[i]) {
				case RED:
					secretCount.red--;
					guessCount.red--;
					break;
				case GREEN:
					secretCount.green--;
					guessCount.green--;
					break;
				case BLUE:
					secretCount.blue--;
					guessCount.blue--;
					break;
				case YELLOW:
					secretCount.yellow--;
					guessCount.yellow--;
					break;
				case ORANGE:
					secretCount.orange--;
					guessCount.orange--;
					break;
				case PURPLE:
					secretCount.purple--;
					guessCount.purple--;
					break;
				default:
					break;
			}
		}
	}

	// Count correct colors in wrong positions
	result.correctColor += std::min(secretCount.red, guessCount.red);
	result.correctColor += std::min(secretCount.green, guessCount.green);
	result.correctColor += std::min(secretCount.blue, guessCount.blue);
	result.correctColor += std::min(secretCount.yellow, guessCount.yellow);
	result.correctColor += std::min(secretCount.orange, guessCount.orange);
	result.correctColor += std::min(secretCount.purple, guessCount.purple);

	return result;
}

TrialResult checkGuess(std::string secret, std::vector<Color> guess) {
	return checkGuess(charsToColors(secret[0], secret[2], secret[4], secret[6]),
										guess);
}
}	 // namespace GameUtils