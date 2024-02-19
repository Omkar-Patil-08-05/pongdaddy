#include <iostream>
#include <ostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8080

int main (int argc, char *argv[]) {
    int cfd = socket(AF_INET, SOCK_STREAM, 0);
    if (cfd < 0) {
        std::cout << "[ERROR] socket creation failed" << std::endl;
    }
    sockaddr_in csoc;
    csoc.sin_family = AF_INET;
    csoc.sin_port = htons(PORT);
    inet_pton(AF_INET, ("10.1.21.146"),&csoc.sin_addr.s_addr);
    if (connect(cfd, (sockaddr*)&csoc, sizeof(csoc)) < 0){
        std::cerr << "[ERROR] Failed to connect to server" << std::endl;
        return 0;
    }
    else{
        std::cout << "[INFO] Client connected to server" << std::endl;
    }
    char buf[400];
    std::cout << "[PROMPT] Enter the data to send : ";
    std::cin.getline(buf,400);
    int msg = send(cfd, buf, 400, 0);
    return 0;
}
