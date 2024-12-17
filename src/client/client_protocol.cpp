#include "client_protocol.hpp"

#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>
#include <sys/ioctl.h>

#include <cstring>
#include <iostream>

bool validateIp(const char *ip) {
	char tmp[sizeof(struct in_addr)];
	if (inet_pton(AF_INET, ip, tmp) == 0) return false;
	return true;
}

bool validatePort(const char *port) {
	errno = 0;
	char *end_ptr{};
	long port_number = std::strtol(port, &end_ptr, 10);
	if (port_number == 0) return false;
	if (errno == ERANGE or port_number < 0 or port_number > (1 << 16) - 1)
		return false;
	return true;
}

void setUdpSocket(std::string ip, std::string port, int *fd,
									struct addrinfo **addr) {
	const char *ip_cstr = ip.c_str(), *port_cstr = port.c_str();

	if ((*fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		std::cerr << "Failed to create the UDP socket" << std::endl;	// DEBUG
		std::perror("[DEBUG] socket");																// DEBUG
		std::cout << "Closing down the application..." << std::endl;
		exit(EXIT_SUCCESS);
	}

	struct addrinfo hints;
	std::memset(&hints, 0, sizeof(hints));
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_family = AF_INET;

	int errcode = getaddrinfo(ip_cstr, port_cstr, &hints, addr);
	if (errcode != 0) {
		std::cerr << "Failed to get the Game Server address" << std::endl;	// DEBUG
		std::cerr << "[DEBUG] getaddrinfo: " << gai_strerror(errcode)
							<< std::endl;	 // DEBUG
		std::cout << "Closing down the application..." << std::endl;
		exit(EXIT_SUCCESS);
	}
}

bool sendUdpPacket(int fd, std::string packet, struct addrinfo addr) {
	if (sendto(fd, packet.c_str(), packet.size(), 0, addr.ai_addr,
						 addr.ai_addrlen) == -1) {
		std::cerr << "Failed to send a request to the Game Server"
							<< std::endl;					// DEBUG
		std::perror("[DEBUG] sendto");	// DEBUG
		return false;
	}
	return true;
}

bool receiveUdpPacket(int fd, std::string &packet) {
	ssize_t read =
			recvfrom(fd, packet.data(), packet.size(), 0, nullptr, nullptr);
	if (read == -1) {
		std::cerr << "Failed to receive a reply from the Game Server"
							<< std::endl;						// DEBUG
		std::perror("[DEBUG] recvfrom");	// DEBUG
		return false;
	}
	packet = packet.substr(0, (size_t)read);
	return true;
}

bool waitUdpPacket(int fd, std::string &packet) {
	fd_set set;
	FD_ZERO(&set);
	FD_SET(fd, &set);
	struct timeval timeout;
	int ready;

	memset(&timeout, 0, sizeof(timeout));
	timeout.tv_sec = UDP_TIMEOUT;

	ready = select(FD_SETSIZE, &set, NULL, NULL, &timeout);
	switch (ready) {
		case 0:
			std::cout << "Timeout" << std::endl;	// DEBUG
			return false;
		case -1:
			std::perror("[DEBUG] select");	// DEBUG
			return false;
		default:
			return receiveUdpPacket(fd, packet);
	}
}

Packet sendUdpAndWait(int fd, std::string request, std::string &reply,
											struct addrinfo addr) {
	for (int i = 0; i < UDP_MAX_TRIES; i++) {
		if (!sendUdpPacket(fd, request, addr)) return NOT_SENT;
		if (waitUdpPacket(fd, reply)) return GOOD;
		if (!KeepRunning) return INTERRUPT;
	}
	std::cerr << "Server did not reply to the request" << std::endl;
	return NOT_REPLIED;
}

bool setTcpSocket(std::string ip, std::string port, int *fd) {
	const char *ip_cstr = ip.c_str(), *port_cstr = port.c_str();

	if ((*fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		std::cerr << "Failed to create the TCP socket" << std::endl;	// DEBUG
		std::perror("[DEBUG] socket");																// DEBUG
		return false;
	}

	struct addrinfo hints, *res;
	std::memset(&hints, 0, sizeof(hints));
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_family = AF_INET;

	int errcode = getaddrinfo(ip_cstr, port_cstr, &hints, &res);
	if (errcode != 0) {
		std::cerr << "Failed to get the Game Server address" << std::endl;	// DEBUG
		std::cerr << "[DEBUG] getaddrinfo: " << gai_strerror(errcode)
							<< std::endl;	 // DEBUG
		return false;
	}

	if (connect(*fd, res->ai_addr, res->ai_addrlen) == -1) {
		std::cerr << "Failed to connect with the Game Server"
							<< std::endl;					 // DEBUG
		std::perror("[DEBUG] connect");	 // DEBUG
		return false;
	}

	return true;
}

bool sendTcpPacket(int fd, std::string packet) {
	size_t nleft = packet.size();
	ssize_t nwritten;
	char *ptr = packet.data();
	while (nleft > 0) {
		nwritten = write(fd, ptr, nleft);
		if (nwritten == -1) {
			std::cerr << "Failed to send the request to the Game Server"
								<< std::endl;				 // DEBUG
			std::perror("[DEBUG] write");	 // DEBUG
			return false;
		}
		nleft -= (size_t)nwritten;
		ptr += nwritten;
	}
	return true;
}

bool receiveTcpPacket(int fd, std::string &reply) {
	size_t nleft;
	ssize_t nread;
	if (ioctl(fd, FIONREAD, &nleft) == -1) {
		std::cerr << "Failed to get TCP reply size" << std::endl;
		std::perror("[DEBUG] ioctl");	 // DEBUG
		close(fd);
		return false;
	};
	reply.resize(nleft);
	char *ptr = reply.data();
	while (nleft > 0) {
		nread = read(fd, ptr, nleft);
		if (nread == -1) {
			std::cerr << "Failed to receive reply from the Game Server GS"
								<< std::endl;				// DEBUG
			std::perror("[DEBUG] read");	// DEBUG
			close(fd);
			return false;
		}
		nleft -= (size_t)nread;
		ptr += nread;
	}
	close(fd);
	return true;
}

bool waitTcpPacket(int fd, std::string &buf) {
	fd_set set;
	FD_ZERO(&set);
	FD_SET(fd, &set);
	struct timeval timeout;
	int ready;

	memset(&timeout, 0, sizeof(timeout));
	timeout.tv_sec = TCP_TIMEOUT;

	ready = select(FD_SETSIZE, &set, NULL, NULL, &timeout);
	switch (ready) {
		case 0:
			std::cout << "Timeout" << std::endl;	// DEBUG
			close(fd);
			return false;
		case -1:
			std::perror("[DEBUG] select");	// DEBUG
			return false;
		default:
			return receiveTcpPacket(fd, buf);
	}
}

Packet sendTcpAndWait(std::string request, std::string &reply, std::string ip,
											std::string port) {
	int fd;
	if (!setTcpSocket(ip, port, &fd)) return NOT_SENT;
	if (!sendTcpPacket(fd, request)) return NOT_SENT;
	if (waitTcpPacket(fd, reply)) return GOOD;
	if (!KeepRunning) return INTERRUPT;
	std::cerr << "Server did not reply to the request" << std::endl;
	return NOT_REPLIED;
}
