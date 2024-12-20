#include "aux.hpp"

#include "game.hpp"

Player::Player(std::string s) {
	plid = s;
};

std::string Player::getPlid() {
	return plid;
}

bool Player::getGame() {
	return game;
}

int Player::getTime() {
	return time;
}

std::vector<Color> Player::getCode() {
	return code;
}

int Player::getTrials() {
	return trials;
}

void Player::incrementTrials() {
	trials++;
}

int Player::compareTrials(int t) {
	return t == trials;
}

void Player::startGame(int t) {
	game = true;
	time = t;
	code = GameUtils::generateCode();
	trials = 0;
}

void Player::startGame(int t, std::vector<Color> c) {
	game = true;
	time = t;
	code = c;
	trials = 0;
}

void Player::endGame() {
	game = false;
	time = 0;
	trials = 0;
	code.clear();
}

std::string GameFile::createGameFileName(const std::string& plid) {
	return "GAME_" + plid + ".txt";
}

bool GameFile::setupDirectories() {
	if (system(("mkdir -p " + std::string(GAMES_DIR)).c_str()) == -1)
		return false;
	if (system(("mkdir -p " + std::string(SCORES_DIR)).c_str()) == -1)
		return false;
	return true;
}

bool GameFile::saveGameStart(const std::string& plid, char mode,
														 const std::string& code, int time) {
	if (!setupDirectories()) return false;

	std::string filename = GAMES_DIR + createGameFileName(plid);
	std::ofstream file(filename);
	if (!file) return false;

	// Get current time
	auto now = std::chrono::system_clock::now();
	auto now_time_t = std::chrono::system_clock::to_time_t(now);
	std::tm* now_tm = std::localtime(&now_time_t);

	// Format: PPPPPP M CCCC T YYYY-MM-DD HH:MM:SS s
	file << plid << " " << mode << " " << code << " " << time << " "
			 << std::put_time(now_tm, "%Y-%m-%d %H:%M:%S") << " "
			 << std::chrono::duration_cast<std::chrono::seconds>(
							now.time_since_epoch())
							.count()
			 << "\n";

	return true;
}

bool GameFile::saveGameTrial(const std::string& plid, const std::string& guess,
														 int correct, int wrong) {
	std::string filename = GAMES_DIR + createGameFileName(plid);
	std::ofstream file(filename, std::ios::app);
	if (!file) return false;

	auto now = std::chrono::system_clock::now();
	auto start_time = getGameStartTime(plid);
	auto elapsed =
			std::chrono::duration_cast<std::chrono::seconds>(now - start_time)
					.count();

	// Format: T: CCCC B W s
	file << "T: " << guess << " " << correct << " " << wrong << " " << elapsed
			 << "\n";

	return true;
}

bool GameFile::checkTrial(const std::string& plid, const std::string& guess) {
	std::string filename = GAMES_DIR + createGameFileName(plid);
	std::ifstream file(filename);
	if (!file) return false;	// If unable to open the file, return false

	std::string line;
	// Skip the first line (initial game information)
	std::getline(file, line);

	// Check each line starting with "T:"
	while (std::getline(file, line)) {
		if (line.substr(0, 2) == "T:") {
			// Extract the trial code (after "T: " and before the next space)
			std::string trial_guess = line.substr(3, line.find(' ', 3) - 3);

			// If a matching trial is found, return true
			if (trial_guess == guess) {
				return true;
			}
		}
	}

	return false;	 // If no matching trial is found, return false
}

bool GameFile::checkTimeExceeded(const std::string& plid) {
	std::string filename = GAMES_DIR + createGameFileName(plid);
	std::ifstream file(filename);
	if (!file)
		return true;	// If can't open file, assume time exceeded for safety

	std::string line;
	if (!std::getline(file, line))
		return true;	// If can't read first line, assume time exceeded

	// Get initial timestamp and time limit from first line
	// Format: PPPPPP M CCCC T YYYY-MM-DD HH:MM:SS s
	try {
		// Extract time limit (T)
		size_t timePos = line.find(' ');				// Skip PLID
		timePos = line.find(' ', timePos + 1);	// Skip mode
		timePos = line.find(' ', timePos + 1);	// Skip code
		size_t nextSpace = line.find(' ', timePos + 1);
		int timeLimit =
				std::stoi(line.substr(timePos + 1, nextSpace - timePos - 1));

		// Extract initial timestamp (s)
		size_t timestampPos = line.rfind(' ');
		long long startTime = std::stoll(line.substr(timestampPos + 1));

		// Get current time
		auto now = std::chrono::system_clock::now();
		long long currentTime =
				std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch())
						.count();

		// Check if elapsed time exceeds limit
		return (currentTime - startTime) > timeLimit;
	} catch (...) {
		return true;	// If any parsing error occurs, assume time exceeded for
									// safety
	}
}

int GameFile::getGameTime(const std::string& plid) {
	std::string filename = GAMES_DIR + createGameFileName(plid);
	std::ifstream file(filename);
	if (!file) return -1;

	std::string line;
	if (!std::getline(file, line)) return -1;

	try {
		// Skip to time value (after PLID, mode, and code)
		size_t timePos = line.find(' ');				// Skip PLID
		timePos = line.find(' ', timePos + 1);	// Skip mode
		timePos = line.find(' ', timePos + 1);	// Skip code
		size_t nextSpace = line.find(' ', timePos + 1);
		return std::stoi(line.substr(timePos + 1, nextSpace - timePos - 1));
	} catch (...) {
		return -1;
	}
}

bool GameFile::finishGame(const std::string& plid, char outcome) {
	std::string filename = GAMES_DIR + createGameFileName(plid);

	// Lê as informações necessárias do arquivo antes de movê-lo
	std::string secret_code;
	int num_trials = 0;
	char mode = 'P';

	std::ifstream file(filename);
	if (file) {
		std::string first_line;
		if (std::getline(file, first_line)) {
			// Parse primeira linha: PPPPPP M CCCC T YYYY-MM-DD HH:MM:SS s
			std::istringstream iss(first_line);
			std::string plid_str;
			iss >> plid_str >> mode >> secret_code;

			// Conta número de tentativas (linhas começando com "T:")
			std::string line;
			while (std::getline(file, line)) {
				if (line.substr(0, 2) == "T:") {
					num_trials++;
				}
			}
		}
		file.close();
	}

	// Cria diretório do jogador se não existir
	std::string playerDir = GAMES_DIR + plid + "/";
	system(("mkdir -p " + playerDir).c_str());

	// Cria nome do arquivo com timestamp
	auto now = std::chrono::system_clock::now();
	auto now_tm = std::chrono::system_clock::to_time_t(now);
	std::tm* ltm = std::localtime(&now_tm);

	char timestamp[20];
	std::strftime(timestamp, sizeof(timestamp), "%Y%m%d_%H%M%S", ltm);

	std::string newFilename =
			playerDir + timestamp + "_" + std::string(1, outcome) + ".txt";

	// Adiciona linha final com tempo de término
	std::ofstream outfile(filename, std::ios::app);
	if (!outfile) return false;

	outfile << std::put_time(ltm, "%Y-%m-%d %H:%M:%S") << " "
					<< std::chrono::duration_cast<std::chrono::seconds>(
								 now - getGameStartTime(plid))
								 .count()
					<< "\n";
	outfile.close();
	std::cout << "Game finished\n";
	std::cout << "Trials: " << num_trials << std::endl;
	std::cout << "Secret code: " << secret_code << std::endl;
	std::cout << "Mode: " << mode << std::endl;
	std::cout << "Outcome: " << outcome << std::endl;

	// Se o jogo foi ganho (outcome == 'W'), salva o score
	if (outcome == 'W') {
		saveScore(plid, secret_code, num_trials + 1, mode);
	}

	// Move o arquivo
	return rename(filename.c_str(), newFilename.c_str()) == 0;
}

bool GameFile::saveScore(const std::string& plid,
												 const std::string& secret_code, int num_trials,
												 char mode) {
	// Calcula o score baseado no número de tentativas
	int score = calculateScore(num_trials);

	// Cria o nome do arquivo no formato (score)_PLID_DDMMYYYY_HHMMSS.txt
	std::string filename = createScoreFileName(plid, score);

	// Cria o diretório SCORES se não existir
	system(("mkdir -p " + std::string(SCORES_DIR)).c_str());

	// Abre o arquivo para escrita
	std::string full_path = std::string(SCORES_DIR) + filename;
	std::ofstream file(full_path);
	if (!file) return false;

	// Escreve a linha com o formato: SSS PPPPPP CCCC N mode
	file << std::setfill('0') << std::setw(3) << score << " " << plid << " "
			 << secret_code << " " << num_trials << " " << mode;

	return true;
}

int GameFile::calculateScore(int num_trials) {
	// Score é inversamente proporcional ao número de tentativas
	// Por exemplo: começando com 100 e diminuindo 10 por tentativa
	int score = 100 - ((num_trials - 1) * 10);
	if (score < 0) score = 0;
	return score;
}

std::string GameFile::createScoreFileName(const std::string& plid, int score) {
	// Obtém a data e hora atual
	auto now = std::chrono::system_clock::now();
	auto now_time_t = std::chrono::system_clock::to_time_t(now);
	std::tm* now_tm = std::localtime(&now_time_t);

	// Formata o nome do arquivo
	std::stringstream ss;
	ss << std::setfill('0') << std::setw(3) << score << "_" << plid << "_"
		 << std::put_time(now_tm, "%d%m%Y_%H%M%S") << ".txt";

	return ss.str();
}

std::chrono::system_clock::time_point GameFile::getGameStartTime(
		const std::string& plid) {
	std::string filename = GAMES_DIR + createGameFileName(plid);
	std::ifstream file(filename);
	if (!file) return std::chrono::system_clock::now();	 // fallback

	std::string line;
	if (std::getline(file, line)) {
		// Parse the timestamp from the first line
		// Format: PPPPPP M CCCC T YYYY-MM-DD HH:MM:SS s
		std::string timestamp = line.substr(line.rfind(' ') + 1);
		try {
			long long seconds = std::stoll(timestamp);
			return std::chrono::system_clock::time_point(
					std::chrono::seconds(seconds));
		} catch (...) {
			return std::chrono::system_clock::now();	// fallback on parse error
		}
	}

	return std::chrono::system_clock::now();	// fallback if file is empty
}

std::string getCmdName(Command cmd) {
	switch (cmd) {
		case CMD_ERR:
			return std::string("ERR");
		case CMD_SNG:
			return std::string("SNG");
		case CMD_TRY:
			return std::string("TRY");
		case CMD_QUT:
			return std::string("QUT");
		case CMD_DBG:
			return std::string("DBG");
		case CMD_STR:
			return std::string("STR");
		case CMD_SSB:
			return std::string("SSB");
		default:
			return std::string("ERR");
	}
}
