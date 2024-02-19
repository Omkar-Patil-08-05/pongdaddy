#include <iostream>
#include <arpa/inet.h>
#include <iterator>
#include <sys/socket.h>
#include <thread>
#include <sys/types.h>
#include <vector>
#include <netinet/in.h>
#include <unistd.h>

std::vector<std::thread> proc;
void process(int con){
    float buf[1];
    while (true) {
        int msg = recv(con, buf, sizeof(buf), 0);
        if(msg == 0) break;
        std::cout << "[SERVER] " << buf[0] << std::endl;
//        for (std::thread& client : proc) {
//            if (client.native_handle() != con) {
//                send(client.native_handle(), buf, sizeof(buf), 0);
//            }
//        }
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
