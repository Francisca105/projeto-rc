#include "protocol.hpp"

#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>

bool receiveUdp(std::string &packet, int fd, Address *addr) {
	ssize_t rcvd;
	if ((rcvd = recvfrom(fd, packet.data(), packet.size(), 0, &addr->addr,
											 &addr->addrlen)) == -1) {
		std::cerr << "Error receiving TCP packet" << std::endl;
		std::perror("[DEBUG] recvfrom");	// DEBUG
		return false;
	}
	packet.resize((size_t)rcvd);
	return true;
}

bool sendUdp(std::string packet, int fd, Address addr) {
	if (sendto(fd, packet.data(), packet.size(), 0, &addr.addr, addr.addrlen) ==
			-1) {
		std::cerr << "Error sending UDP packet" << std::endl;
		std::perror("[DEBUG] sendto");	// DEBUG
		return false;
	}
	return true;
}

bool receiveTcp(std::string &packet, int fd) {
	if (!readString(packet, fd, sizeof("SSB\n") - 1)) return false;
	if (!packet.compare("SSB\n")) {
		return true;
	} else {
		if (!readString(packet, fd, sizeof("PPPPPP\n") - 1)) return false;
	}
	return true;
}

bool readString(std::string &buf, int fd, size_t size) {
	ssize_t nread;
	std::string data(size, 0);
	char *ptr = data.data();
	while (size > 0) {
		nread = read(fd, ptr, size);
		if (nread == -1) {
			std::cerr << "Failed to receive the reply from the server" << std::endl;
			std::perror("[DEBUG] write");	 // DEBUG
			return false;
		}
		size -= (size_t)nread;
		ptr += nread;
	}
	buf += data;
	return true;
}

bool sendTcp(std::string packet, int fd) {
	size_t size = packet.size();
	ssize_t nwritten;
	char *ptr = packet.data();
	while (size > 0) {
		nwritten = write(fd, ptr, size);
		if (nwritten == -1) {
			std::cerr << "Failed to send a request to the server" << std::endl;
			std::perror("[DEBUG] write");	 // DEBUG
			return false;
		}
		size -= (size_t)nwritten;
		ptr += nwritten;
	}
	return true;
}