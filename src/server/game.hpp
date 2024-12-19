#include <string>
#include <vector>

#include "../common/common.hpp"

#ifndef UTILS_H
#define UTILS_H

typedef struct {
	int correctColorAndPosition;
	int correctColor;
} TrialResult;

typedef struct {
	int red;
	int green;
	int blue;
	int yellow;
	int orange;
	int purple;
} ColorCount;

// Utility functions for the game server
namespace GameUtils {

/*   Verifications   */

// Returns whether the color is valid
int isColorValid(char c);

// Returns whether the guess is valid
int isGuessValid(char c1, char c2, char c3, char c4);

// Returns whether the trials are exceeded
int isTrialsExceeded(int trials);

/*   Auxiliary functions   */

// Converts a color to its char representation
char colorToChar(Color color);

// Counts the number of colors in a vector
ColorCount countColors(std::vector<Color> colors);

// Converts chars to a vector of colors
std::vector<Color> charsToColors(char c1, char c2, char c3, char c4);

/*   Game logic   */

// Generates a sequence of random colors
std::vector<Color> generateRandomColors();

TrialResult checkGuess(std::vector<Color> secret, std::vector<Color> guess);
}	 // namespace GameUtils

#endif	// UTILS_H