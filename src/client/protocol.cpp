#include "protocol.hpp"

#include <cstring>
#include <iostream>

bool sendUdp(std::string packet, int fd, struct addrinfo addr) {
	if (sendto(fd, packet.c_str(), packet.size(), 0, addr.ai_addr,
						 addr.ai_addrlen) == -1) {
		std::cerr << "Failed to send a request to the server" << std::endl;
		std::perror("[DEBUG] sendto");	// DEBUG
		return false;
	}
	return true;
}

bool receiveUdp(std::string &packet, int fd) {
	ssize_t rcvd;
	if ((rcvd = recvfrom(fd, packet.data(), packet.size(), 0, nullptr,
											 nullptr)) == -1) {
		std::cerr << "Failed to receive a reply from the server" << std::endl;
		std::perror("[DEBUG] recvfrom");	// DEBUG
		return false;
	}
	packet.resize((size_t)rcvd);
	return true;
}

bool waitUdp(std::string &packet, int fd) {
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
			if (errno != EINTR) std::perror("[DEBUG] select");	// DEBUG
			return false;
		default:
			return receiveUdp(packet, fd);
	}
}

bool sendUdpAndReceive(std::string request, std::string &reply, int fd,
											 struct addrinfo addr) {
	for (int i = 0; i < UDP_MAX_TRIES; i++) {
		if (!sendUdp(request, fd, addr)) return false;
		if (waitUdp(reply, fd)) return true;
		if (!KeepRunning) return false;
	}
	std::cerr << "Server did not reply to the request" << std::endl;
	return true;
}

bool setTcpSocket(std::string ip, std::string port, int *fd) {
	if ((*fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		std::cerr << "Failed to create the TCP socket" << std::endl;
		std::perror("[DEBUG] socket");	// DEBUG
		return false;
	}

	struct timeval timeout;
	timeout.tv_sec = TCP_TIMEOUT;
	timeout.tv_usec = 0;

	if (setsockopt(*fd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) < 0) {
		std::cerr << "Failed to set TCP socket send timeout" << std::endl;
		perror("[DEBUG] setsockopt");	 // DEBUG
		close(*fd);
		return false;
	}

	if (setsockopt(*fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
		std::cerr << "Failed to set TCP socket send timeout" << std::endl;
		perror("[DEBUG] setsockopt");	 // DEBUG
		close(*fd);
		return false;
	}

	struct addrinfo hints, *res;
	std::memset(&hints, 0, sizeof(hints));
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_family = AF_INET;

	int errcode = getaddrinfo(ip.c_str(), port.c_str(), &hints, &res);
	if (errcode != 0) {
		std::cerr << "Failed to get the server address" << std::endl;
		std::cerr << "[DEBUG] getaddrinfo: " << gai_strerror(errcode)
							<< std::endl;	 // DEBUG
		close(*fd);
		return false;
	}

	if (connect(*fd, res->ai_addr, res->ai_addrlen) == -1) {
		std::cerr << "Failed to connect with the server" << std::endl;
		std::perror("[DEBUG] connect");	 // DEBUG
		close(*fd);
		return false;
	}

	return true;
}

bool sendTcp(std::string packet, int fd) {
	size_t nleft = packet.size();
	ssize_t nwritten;
	char *ptr = packet.data();
	while (nleft > 0) {
		nwritten = write(fd, ptr, nleft);
		if (nwritten == -1) {
			std::cerr << "Failed to send the request to the server" << std::endl;
			std::perror("[DEBUG] write");	 // DEBUG
			close(fd);
			return false;
		}
		nleft -= (size_t)nwritten;
		ptr += nwritten;
	}
	return true;
}

bool receiveTcp(std::string &packet, int fd) {
	if (!readString(packet, fd)) return false;
	if (!compareChar(packet, ' ')) return false;
	if (!readString(packet, fd)) return false;
	if (compareChar(packet, ' ')) {
		if (!readString(packet, fd)) return false;
		if (!compareChar(packet, ' ')) return false;
		size_t fsize;
		if (!readUint(packet, fd, &fsize)) return false;
		if (!compareChar(packet, ' ')) return false;
		if (!readString(packet, fd, fsize)) return false;
	}
	return compareChar(packet, '\n');
}

bool readChar(std::string &buf, int fd) {
	ssize_t nwritten;
	char c;
	nwritten = read(fd, &c, 1);
	if (nwritten == -1) {
		std::cerr << "Failed to receive a reply from the server" << std::endl;
		std::perror("[DEBUG] read");	// DEBUG
		close(fd);
		return false;
	}
	buf += c;
	return true;
}

bool readString(std::string &buf, int fd) {
	char c;
	while (true) {
		if (!readChar(buf, fd)) return false;
		c = buf.back();
		if (!std::isalnum(c) and c != '.' and c != '-' and c != '_') break;
	}
	return true;
}

bool readString(std::string &buf, int fd, size_t size) {
	ssize_t nwritten;
	std::string data(size, 0);
	char *ptr = data.data();
	while (size > 0) {
		nwritten = read(fd, ptr, size);
		if (nwritten == -1) {
			std::cerr << "Failed to receive the reply from the server" << std::endl;
			std::perror("[DEBUG] write");	 // DEBUG
			close(fd);
			return false;
		}
		size -= (size_t)nwritten;
		ptr += nwritten;
	}
	buf += data;
	return true;
}

bool compareChar(std::string buf, char c) {
	return c == buf.back();
}

bool readUint(std::string &buf, int fd, size_t *n) {
	char c;
	size_t i = 0;
	while (true) {
		if (!readChar(buf, fd)) return false;
		c = buf.back();
		if (!std::isdigit(c)) break;
		i++;
	}
	std::string num_s = buf.substr(buf.size() - i - 1, i);
	char *end_ptr{};
	long num = std::strtol(num_s.c_str(), &end_ptr, 10);
	if (num <= 0 or num > MAX_FILESIZE) return false;
	*n = (size_t)num;
	return true;
}

bool waitTcpPacket(std::string &buf, int fd) {
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
			return receiveTcp(buf, fd);
	}
}

bool sendTcpAndReceive(std::string request, std::string &reply, std::string ip,
											 std::string port) {
	int fd;
	if (!setTcpSocket(ip, port, &fd)) return false;
	if (!sendTcp(request, fd)) return false;
	bool result = waitTcpPacket(reply, fd);
	close(fd);
	return result;
}
