#include "transport.hpp"

// #include <netdb.h>
// #include <sys/ioctl.h>
// #include <sys/socket.h>
// #include <sys/types.h>
// #include <unistd.h>

// #include <cstring>
// #include <iostream>

// #include "../common/constants.hpp"

// int setUdpSocket(std::string ip, std::string port, struct addrinfo **addr,
// 								 bool server) {
// 	const char *ip_cstr = ip.c_str(), *port_cstr = port.c_str();
// 	int fd;
// 	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
// 		std::cerr << "Could not set the UDP socket.\n";
// 		std::perror("Error - socket");
// 		exit(1);
// 	}
// 	struct addrinfo hints;
// 	std::memset(&hints, 0, sizeof(hints));
// 	hints.ai_socktype = SOCK_DGRAM;
// 	hints.ai_family = AF_INET;
// 	if (server) hints.ai_flags = AI_PASSIVE;
// 	int errcode = getaddrinfo(ip_cstr, port_cstr, &hints, addr);
// 	if (errcode != 0) {
// 		std::cerr << "Could not get the address information.\n"
// 							<< "Error - getaddrinfo: " << gai_strerror(errcode) << "\n";
// 		exit(1);
// 	}
// 	if (server and bind(fd, (*addr)->ai_addr, (*addr)->ai_addrlen) == -1) {
// 		std::cerr << "Could not bind the UDP socket.\n";
// 		std::perror("bind");
// 		exit(1);
// 	}
// 	return fd;
// }

// void sendUdpPacket(int fd, std::string packet, struct addrinfo addr) {
// 	if (sendto(fd, packet.c_str(), packet.size(), 0, addr.ai_addr,
// 						 addr.ai_addrlen) == -1) {
// 		std::cerr << "Could not send UDP packet.\n";
// 		std::perror("Error - sendto");
// 		exit(1);
// 	}
// }

// void receiveUdpPacket(int fd, std::string &reply, Address *addr) {
// 	ssize_t nbytes =
// 			addr == nullptr
// 					? recvfrom(fd, reply.data(), MAX_UDP_REPLY, 0, nullptr, nullptr)
// 					: recvfrom(fd, reply.data(), MAX_UDP_REPLY, 0,
// 										 (struct sockaddr *)&addr->addr, &addr->addrlen);
// 	if (nbytes == -1) {
// 		std::cerr << "Could not receive UDP packet.\n";
// 		std::perror("Error - recvfrom");
// 		exit(1);
// 	}
// 	reply = reply.substr(0, (size_t)nbytes);
// }

// bool waitPacket(int fd, std::string &buf, Address *addr) {
// 	fd_set set;
// 	FD_ZERO(&set);
// 	FD_SET(fd, &set);
// 	struct timeval timeout;
// 	int ready;

// 	for (uint i = 0; i < MAX_UDP_TRIES; i++) {
// 		memset(&timeout, 0, sizeof(timeout));
// 		timeout.tv_sec = UDP_TIMEOUT;

// 		ready = select(FD_SETSIZE, &set, NULL, NULL, &timeout);
// 		switch (ready) {
// 			case 0:
// 				std::cout << "Timeout\n";
// 				return false;
// 			case -1:
// 				std::perror("Error - select");
// 				exit(1);
// 			default:
// 				receiveUdpPacket(fd, buf, addr);
// 				return true;
// 		}
// 	}
// 	return false;
// }

// bool sendUdpAndWait(int fd, std::string packet, std::string &reply,
// 										struct addrinfo addr, Address *sender) {
// 	for (uint i = 0; i < MAX_UDP_TRIES; i++) {
// 		sendUdpPacket(fd, packet, addr);
// 		if (waitPacket(fd, reply, sender)) return true;
// 	}
// 	return false;
// }

// int setTcpSocket(std::string ip, std::string port, struct addrinfo **addr,
// 								 bool server) {
// 	const char *ip_cstr = ip.c_str(), *port_cstr = port.c_str();
// 	int fd;
// 	if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
// 		std::cerr << "Could not set the TCP socket.\n";
// 		std::perror("Error - socket");
// 		exit(1);
// 	}
// 	struct addrinfo hints;
// 	std::memset(&hints, 0, sizeof(hints));
// 	hints.ai_socktype = SOCK_STREAM;
// 	hints.ai_family = AF_INET;
// 	if (server) hints.ai_flags = AI_PASSIVE;
// 	int errcode = getaddrinfo(ip_cstr, port_cstr, &hints, addr);
// 	if (errcode != 0) {
// 		std::cerr << "Could not get the address information.\n"
// 							<< "Error - getaddrinfo: " << gai_strerror(errcode) << "\n";
// 		exit(1);
// 	}
// 	if (server and bind(fd, (*addr)->ai_addr, (*addr)->ai_addrlen) == -1) {
// 		std::cerr << "Could not bind the TCP socket.\n";
// 		std::perror("bind");
// 		exit(1);
// 	}
// 	if (server and listen(fd, MAX_CONNECTIONS) == -1) {
// 		std::cerr << "Could not prepare the TCP socket to accept connections.\n";
// 		std::perror("listen");
// 		exit(1);
// 	}
// 	return fd;
// }

// void aaa(int fd, struct addrinfo addr, std::string packet, std::string
// &reply) { 	if (connect(fd, addr.ai_addr, addr.ai_addrlen) == -1) {
// std::cerr
// << "Could not connect with the TCP server.\n"; 		std::perror("connect");
// 		exit(1);
// 	}
// 	std::cout << packet;
// 	char *ptr = packet.data();
// 	size_t nleft = packet.size();
// 	ssize_t nwritten;
// 	while (nleft > 0) {
// 		nwritten = write(fd, ptr, nleft);
// 		if (nwritten == -1) {
// 			std::cerr << "Could not write TCP.\n";
// 			std::perror("write");
// 			close(fd);
// 			exit(1);
// 		}
// 		nleft -= (size_t)nwritten;
// 		ptr += nwritten;
// 	}
// 	fd_set set;
// 	FD_ZERO(&set);
// 	FD_SET(fd, &set);
// 	struct timeval timeout;
// 	int ready;
// 	for (uint i = 0; i < MAX_UDP_TRIES; i++) {
// 		memset(&timeout, 0, sizeof(timeout));
// 		timeout.tv_sec = UDP_TIMEOUT;

// 		ready = select(FD_SETSIZE, &set, NULL, NULL, &timeout);
// 		switch (ready) {
// 			case 0:
// 				std::cout << "Timeout\n";
// 				break;
// 			case -1:
// 				std::perror("Error - select");
// 				close(fd);
// 				exit(1);
// 			default:
// 				int len = 0;
// 				if (ioctl(fd, FIONREAD, &len) == -1) {
// 					std::perror("ioctl");
// 					close(fd);
// 					exit(1);
// 				};
// 				std::cout << "reply size: " << len << "\n";
// 				reply.clear();
// 				reply.reserve((size_t)len);
// 				ptr = reply.data();
// 				nleft = (size_t)len;
// 				while (nleft > 0) {
// 					nwritten = read(fd, ptr, nleft);
// 					if (nwritten == -1) {
// 						std::cerr << "Could not read TCP.\n";
// 						std::perror("write");
// 						close(fd);
// 						exit(1);
// 					}
// 					nleft -= (size_t)nwritten;
// 					ptr += nwritten;
// 				}
// 				close(fd);
// 				return;
// 		}
// 	}
// 	close(fd);
// }
