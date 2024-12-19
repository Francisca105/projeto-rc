#include <iostream>
#include <fstream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int main()
{
    // Create socket
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0)
    {
        std::cerr << "Error creating socket" << std::endl;
        return 1;
    }

    // Configure server address
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);

    // Connect to server
    if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        std::cerr << "Connection error" << std::endl;
        return 1;
    }

    // Open and send file
    std::ifstream file("send.txt", std::ios::binary);
    if (!file.is_open())
    {
        std::cerr << "Error opening file" << std::endl;
        return 1;
    }

    char buffer[1024];
    while (!file.eof())
    {
        file.read(buffer, sizeof(buffer));
        int bytesRead = file.gcount();
        send(clientSocket, buffer, bytesRead, 0);
    }

    // Close everything
    file.close();
    close(clientSocket);

    std::cout << "File sent successfully!" << std::endl;
    return 0;
}