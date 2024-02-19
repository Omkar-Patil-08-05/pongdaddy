#include <cstdio>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <ostream>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8080
#define PIPE "/tmp/pongdaddy"
int main (int argc, char *argv[]) {
    int cfd = socket(AF_INET, SOCK_STREAM, 0);
    if (cfd < 0) {
        std::cout << "[ERROR] socket creation failed" << std::endl;
    }
    sockaddr_in csoc;
    csoc.sin_family = AF_INET;
    csoc.sin_port = htons(PORT);
    inet_pton(AF_INET, ("192.168.1.7"),&csoc.sin_addr.s_addr);
    if (connect(cfd, (sockaddr*)&csoc, sizeof(csoc)) < 0){
        std::cerr << "[ERROR] Failed to connect to server" << std::endl;
        return 0;
    }
    else{
        std::cout << "[INFO] Client connected to server" << std::endl;
    }
    if (access(PIPE, F_OK)==-1) {
       mkfifo(PIPE, 0666); 
    }
    int pfd = open(PIPE, O_RDONLY | O_NONBLOCK);
    if (pfd<0) {
        std::cerr << "[ERROR] Error opening pipe" << std::endl;
        return -1;
    }
    float buf[1];
    float buff[1];
    float y = 0;
    while (1) {
        read(pfd, buf, sizeof(buf));
       std::cout << "[CLIENT] " << buf[0] << std::endl;
        int msg = send(cfd, buf, sizeof(buf), 0);
       // int recev = recv(cfd, buff, sizeof(buff), 0) ;
       // std::cout << "[RECEIVED] " << buff << std::endl;
        usleep(160000);
    }
    close(cfd);
    close(pfd);
    return 0;
}
