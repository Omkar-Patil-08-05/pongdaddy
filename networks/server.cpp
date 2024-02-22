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

#define RPORT 8089
#define WPORT1 8008
#define WPORT2 8009
float buf[2] = {0,0};
float buf1[2] = {0,0};
float buf2[2] = {0,0};
std::vector<std::thread> proc;
std::vector<std::thread> sendlist;
void send_to_client(int sender,float buffer[2]){
    while(true){
        int msg = send(sender, buffer, sizeof(float)*2, 0);
        std::cout << msg << std::endl;
    }
}
void process(int con){
    std::cout << "[POLLING STARTED]" << std::endl;
    while (true) {
        int msg = recv(con, buf, sizeof(buf), 0);
        if(msg == 0) break;
        if (buf[0]==0) {
            std::memcpy(buf1,buf,sizeof(buf));
        }
        else{
            std::memcpy(buf2, buf, sizeof(buf));
            std::cout << "[P2] " << buf2[1] << std::endl;
        }
        //   std::cout << "[SERVER] " << buf[0] << std::endl;
    }
}
int main(){
    int reader = socket(AF_INET, SOCK_STREAM, 0);
    int writer1 = socket(AF_INET, SOCK_STREAM, 0);
    int writer2 = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in readsoc;
    readsoc.sin_family = AF_INET;
    readsoc.sin_port = htons(RPORT);
    readsoc.sin_addr.s_addr = INADDR_ANY;
    //    inet_pton(AF_INET, ("127.0.0.1"), &dat.sin_addr.s_addr);
    if (bind(reader, (sockaddr*)&readsoc, sizeof(readsoc)) < 0 ){
        std::cerr << "[ERROR] Socket Bind Failed" << std::endl;
        close(reader);
        return 0;
    }
    else{
        std::cout << "[INFO] Socket bound" << std::endl;
    }
    if (listen(reader, 2) < 0){
        std::cerr << "[ERROR] Can't listen on socket" << std::endl;
        close(reader);
        return 0;
    }
    else{
        std::cout << "[INFO] Socket listening" << std::endl;
    }
    sockaddr_in writesoc1;
    writesoc1.sin_family = AF_INET;
    writesoc1.sin_port = htons(WPORT1);
    writesoc1.sin_addr.s_addr = INADDR_ANY;
    //    inet_pton(AF_INET, ("127.0.0.1"), &dat.sin_addr.s_addr);
    if (bind(writer1, (sockaddr*)&writesoc1, sizeof(writesoc1)) < 0 ){
        std::cerr << "[ERROR] Socket Bind Failed" << std::endl;
        close(reader);
        return 0;
    }
    else{
        std::cout << "[INFO] Socket bound" << std::endl;
    }
    if (listen(writer1, 2) < 0){
        std::cerr << "[ERROR] Can't listen on socket" << std::endl;
        close(reader);
        return 0;
    }
    else{
        std::cout << "[INFO] Socket listening" << std::endl;
    }
    sockaddr_in writesoc2;
    writesoc2.sin_family = AF_INET;
    writesoc2.sin_port = htons(WPORT2);
    writesoc2.sin_addr.s_addr = INADDR_ANY;
    //    inet_pton(AF_INET, ("127.0.0.1"), &dat.sin_addr.s_addr);
    if (bind(writer2, (sockaddr*)&writesoc2, sizeof(writesoc2)) < 0 ){
        std::cerr << "[ERROR] Socket Bind Failed" << std::endl;
        close(reader);
        return 0;
    }
    else{
        std::cout << "[INFO] Socket bound" << std::endl;
    }
    if (listen(writer2, 2) < 0){
        std::cerr << "[ERROR] Can't listen on socket" << std::endl;
        close(reader);
        return 0;
    }
    else{
        std::cout << "[INFO] Socket listening" << std::endl;
    }
    while (true) {
        int con = accept(reader, NULL, NULL);
        int writecon1 = accept(writer1, NULL, NULL);
        int writecon2 = accept(writer2, NULL, NULL);
        // 2 threads for each client 
        if (con < 0){
            std::cerr << "[ERROR] Socket failed to accept connection" << std::endl;
            return -1;
        }
        else{
            std::cout << "[INFO] Socket Accepted connection from client" << std::endl;
        }
        sendlist.emplace_back(send_to_client,writecon1,buf2);
        sendlist.back().detach();
        sendlist.emplace_back(send_to_client,writecon2,buf1);
        sendlist.back().detach();
        proc.emplace_back(process,con);
        proc.back().detach();

    }
    close(reader);
    close(writer1);
}
