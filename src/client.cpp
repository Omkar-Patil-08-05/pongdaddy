#include <condition_variable>
#include <csignal>
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
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <arpa/inet.h>
#include <vector>
#include "clients.h"
#define WPORT 8089
#define RPORT 8008
#define PIPE "/tmp/pongdaddy"
int wsoc,rsoc;
void send_to_server(int cfd,float buf[2]){
    std::cout << "[SENDING STARTING]" << std::endl;
    while (true) {
        int msg = send(cfd, buf, sizeof(float)*2, 0);
        if (msg<0) {
            perror("send");
        }
    }
}
void receive_from_server(int reader,float readbuf[2]){
    while (true) {
        int msg = recv(reader, readbuf, sizeof(float)*2, 0);
        if (msg==0) break;
        std::cout << readbuf[1] << std::endl;
    }

}
void configure_sockets(sockaddr_in *writesoc, sockaddr_in *readsoc){
    (*writesoc).sin_family = AF_INET;
    (*writesoc).sin_port = htons(WPORT);
    (*readsoc) = *writesoc;
    (*readsoc).sin_port = htons(RPORT);
    inet_pton(AF_INET, ("192.168.1.7"),&(*writesoc).sin_addr.s_addr);
    inet_pton(AF_INET, ("192.168.1.7"),&(*readsoc).sin_addr.s_addr);
}


CONS create_sockets(int* writer, int* reader){
    wsoc = *writer;
    rsoc = *reader;
    if (*reader<0) {
        std::cout << "[ERROR] read socket creation failed" << std::endl;
    }
    if (*writer < 0) {
        std::cout << "[ERROR] write socket creation failed" << std::endl;
    }
    sockaddr_in writesoc,readsoc;
    configure_sockets(&writesoc, &readsoc);
    CONS cons;
    cons.readcon =  connect(*reader, (sockaddr*)&readsoc, sizeof(readsoc));
    cons.writecon = connect(*writer, (sockaddr*)&writesoc, sizeof(writesoc));
    if (cons.readcon < 0 && cons.writecon < 0){
        std::cerr << "[ERROR] Failed to connect to server" << std::endl;
    }
    else{
        std::cout << "[INFO] Client connected to server" << std::endl;
    }
    return cons;
}

