#include <cstring>
#include <iostream>
#include <arpa/inet.h>
#include <iterator>
#include <sys/socket.h>
#include <thread>
#include <sys/types.h>
#include <vector>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 8080

char buffer[1024] = {0};
void comms(int socket) {
    ssize_t valread;
    while ((valread = read(socket, buffer, 1024)) > 0) {
        std::cout << "Received: " << buffer << std::endl;
        send(socket, buffer, valread, 0);
        memset(buffer, 0, sizeof(buffer));
    }
    std::cout << "Connection closed" << std::endl;
    close(socket);
}

int main() {
    int server_fd, connection;
    sockaddr_in address;
    int addrlen = sizeof(address);
    
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    inet_pton(AF_INET, ("192.168.1.7"), &address.sin_addr.s_addr);
    address.sin_port = htons(PORT);

    if (bind(server_fd, (sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 2) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    std::cout << "Server listening on port " << PORT << std::endl;

    while (true) {
        if ((connection = accept(server_fd, (sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }
        
        std::cout << "Connection accepted" << std::endl;

        std::thread t(comms, connection);
        t.detach(); // Detach thread to run independently
    }

    close(server_fd);

    return 0;
}

