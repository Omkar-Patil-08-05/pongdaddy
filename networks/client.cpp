#include <cstdio>
#include <openssl/bio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <ostream>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include <netinet/in.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <arpa/inet.h>
#include <vector>

#define WPORT 8089
#define RPORT 8008
#define PIPE "/tmp/pongdaddy"

float buf[2];
float buff[1];
float y = 0;
void send_to_server(int pfd,int cfd){
    while (true) {
        read(pfd, buf+1, sizeof(buf));
        int msg = send(cfd, buf, sizeof(buf), 0);
    }
}


int main (int argc, char *argv[]) {
    int writer = socket(AF_INET, SOCK_STREAM, 0);
    int reader = socket(AF_INET, SOCK_STREAM, 0);
    if (writer < 0) {
        std::cout << "[ERROR] socket creation failed" << std::endl;
    }
    sockaddr_in writesoc;
    writesoc.sin_family = AF_INET;
    writesoc.sin_port = htons(WPORT);
    sockaddr_in readsoc;
    readsoc = writesoc;
    readsoc.sin_port = htons(RPORT);
    inet_pton(AF_INET, ("192.168.1.7"),&writesoc.sin_addr.s_addr);
    inet_pton(AF_INET, ("192.168.1.7"),&readsoc.sin_addr.s_addr);
    int con =  connect(reader, (sockaddr*)&readsoc, sizeof(readsoc));
    std::cout << con<<std::endl;
    if (connect(writer, (sockaddr*)&writesoc, sizeof(writesoc)) < 0 ){
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
    std::vector<std::thread> channel;
    
    channel.emplace_back(send_to_server,pfd,writer);
    channel.back().detach();
    float readbuf[2] = {-1,-1};
    while (true) {
        int msg = recv(reader, readbuf, sizeof(readbuf), 0);
        if (msg==0) break;
        std::cout << "[CLIENT P2]" <<  readbuf[1] << std::endl;
    }
    // while (1) {
    //   read(pfd, buf, sizeof(buf));
    // std::cout << "[CLIENT] " << buf[0] << std::endl;
    // int msg = send(cfd, buf, sizeof(buf), 0);
    // int recev = recv(cfd, buff, sizeof(buff), 0) ;
    //std::cout << "[RECEIVED] " << buff << std::endl;
    //        usleep(160000);
    // }
    close(writer);
    close(reader);
    close(pfd);
    return 0;
}
