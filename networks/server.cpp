#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <thread>
#include <vector>
#include <netinet/in.h>
#include <unistd.h>

void process(int con){
char buf[400];
while (true) {
    int msg = recv(con, buf, 400, 0);
    std::cout << "Message from Client" << buf << std::endl;
}
}
#define PORT 8080
int main(){
    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in dat;
    dat.sin_family = AF_INET;
    dat.sin_port = htons(PORT);
    dat.sin_addr.s_addr = INADDR_ANY;
    //    inet_pton(AF_INET, ("127.0.0.1"), &dat.sin_addr.s_addr);
    if (bind(sfd, (sockaddr*)&dat, sizeof(dat)) < 0){
        std::cerr << "[ERROR] Socket Bind Failed" << std::endl;
        close(sfd);
        return 0;
    }
    else{
        std::cout << "[INFO] Socket bound" << std::endl;
    }
    if (listen(sfd, 2) < 0){
        std::cerr << "[ERROR] Can't listen on socket" << std::endl;
        close(sfd);
        return 0;
    }
    else{
        std::cout << "[INFO] Socket listening" << std::endl;
    }
    std::vector<std::thread> proc;
    while (true) {
        int con = accept(sfd, NULL, NULL);
        if (con < 0){
            std::cerr << "[ERROR] Socket failed to accept connection" << std::endl;
            return -1;
        }
        else{
            std::cout << "[INFO] Socket Accepted connection from client" << std::endl;
        }
        proc.emplace_back(process,con);
        proc.back().detach();
        
    }
    close(sfd);

}
