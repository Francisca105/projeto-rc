#include "../common/common.hpp"
#include "server.hpp"

int main(int argc, char** argv) {
	std::string port_number = GSPORT;
	bool verbose = false;

	if (argc > 1) {
		bool error = false;
		switch (argc) {
			case 2:
				error = std::string("-v").compare(argv[1]) ? true : false;
				if (!error) verbose = true;
				break;
			case 3:
				error = std::string("-p").compare(argv[1]) ? true : false;
				if (!error) {
					try {
						int n = std::stoi(argv[2]);
						if ((n < 0) or (0 < n and n < 1024) or (n > 65535)) {
							std::cerr
									<< "Port number out of valid range ~(0 + 1024-65535).\n";
							error = true;
						}
					} catch (std::invalid_argument const& ex) {
						std::cerr
								<< "Error converting port number to valid number (integer).\n";
						error = true;
					} catch (std::out_of_range const& ex) {
						std::cerr << "Port number out of valid range ~(0 + 1024-65535).\n";
						error = true;
					}
					if (!error) port_number = argv[2];
				}
				break;
			case 4:
				error = std::string("-p").compare(argv[1]) ? true : false;
				if (!error) {
					try {
						int n = std::stoi(argv[2]);
						if ((n < 0) or (0 < n and n < 1024) or (n > 65535)) {
							std::cerr
									<< "Port number out of valid range ~(0 + 1024-65535).\n";
							error = true;
						}
					} catch (std::invalid_argument const& ex) {
						std::cerr
								<< "Error converting port number to valid number (integer).\n";
						error = true;
					} catch (std::out_of_range const& ex) {
						std::cerr << "Port number out of valid range ~(0 + 1024-65535).\n";
						error = true;
					}
					if (!error) port_number = argv[2];
				}
				if (!error) {
					error = std::string("-v").compare(argv[3]) ? true : false;
					if (!error) verbose = true;
				}
				break;
			default:
				error = true;
		}
		if (error) {
			std::cerr << "usage: ./GS [-p GSport] [-v]\n";
			exit(1);
		}
	}
	if (verbose) std::cout << "The server is running on verbose mode.\n";

	if (runServer(port_number, verbose) == 0)
		std::cout << "\nServer closed successfuly.\n";

	return 0;
}
