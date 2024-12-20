# RC Master Mind Project 2024/2025

Computer Networks (RC) - IST
Group 50

Members:

| 106943 | José Maria de Noronha Wolfango Franco Frazão |
| ------ | -------------------------------------------- |
| 105901 | Francisca Vicente de Almeida                 |

## Overview

This project implements a client-server version of the Master Mind game where:

- Players try to guess a 4-color secret code (e.g. R G B Y)
- Each guess receives feedback with:
- Number of correct colors in correct positions (black pegs)
- Number of correct colors in wrong positions (white pegs)
- Players have maximum 8 trials to guess correctly
- Each game has a maximum duration of 600 seconds (10 minutes)
- Only one active game per player is allowed
- Valid colors are: R (red), G (green), B (blue), Y (yellow), O (orange), P (purple)
- Scoreboard tracks top 10 winning games

## Compiling

The project uses C++ and can be compiled by running `make` in the root directory. This will generate two main executables:

- `player`: The player client application
- `GS`: The game server

## Running the Game Server (GS)

The Game Server can be started using:

```
./GS [-p GSport] [-v]
```

Where:

- `-p GSport`: Optional. Specifies the port number for both UDP and TCP connections. Defaults to 58000+GN, where GN is the group number (50).
- `-v`: Optional. Enables verbose mode, showing detailed information about received requests.

## Running the Player Application

The player application can be started using:

```
./player [-n GSIP] [-p GSport]
```

Where:

- `-n GSIP`: Optional. IP address of the Game Server. Defaults to localhost
- `-p GSport`: Optional. Port number of the Game Server. Defaults to 58000+GN (50)

## Player Commands

Once the player is running, the following commands are available:

- `start PLID max_playtime`: Start a new game with player ID and maximum playtime (in seconds, max 600)
- `try C1 C2 C3 C4`: Make a guess with four colors
- `show_trials` or `st`: View previous attempts
- `scoreboard` or `sb`: View the top 10 scores
- `quit`: End the current game
- `exit`: Exit the application
- `debug PLID max_playtime C1 C2 C3 C4`: Start a game in debug mode with a specified secret code

Valid colors are: `R` (red), `G` (green), `B` (blue), `Y` (yellow), `O` (orange), `P` (purple)

## Technical Details

The project uses both UDP and TCP protocols:

- Developed in C++17
- Uses socket programming (UDP and TCP)
- UDP: For game-related communications (start game, make guesses, etc.)
- TCP: For file transfers (viewing trials and scoreboard)
- Both protocols operate on the same port
- Server can handle multiple simultaneous connections
- Implements thread-safe data structures for concurrent game sessions
- Handles graceful shutdown with SIGINT signal

### Network Protocol Specifications

- All messages end with a newline character ("\n")
- Fields in messages are separated by single spaces
- PLID must be a 6-digit number
- File names are limited to 24 alphanumeric characters (including "-", "\_", ".")
- Maximum file size is 2 KiB

## Error Handling

The application handles various error conditions both in Client and in the Game Server:

- Invalid protocol messages
- Network communication errors
- Timeouts
- Invalid game states
- Invalid player IDs
- Invalid color codes

## Files

- `games/`: Directory containing current games and finished games
- `games/`: Directory of the scoreboard information
- `src/client/*.cpp`, `*.hpp` - Player application implementation
- `src/common/*.cpp`, `*.hpp` - Shared code between client and server
- `src/server/*.cpp`, `*.hpp` - Game Server implementation
- `Makefile` - Build instructions
- `README.md` - This file
