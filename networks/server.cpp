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
void send_to_client(int sender){
    while(true){
        if (buf[0]==0) {
        //    std::cout << "[P1] " << buf1[1] << std::endl;
        }
        else{
            int msg = send(sender, buf2, sizeof(buf2), 0);
        }

      //  int msg = send(sender, buf, sizeof(buf), 0);
    }
}
void process(int con){
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
    int writer = socket(AF_INET, SOCK_STREAM, 0);
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
    sockaddr_in writesoc;
    writesoc.sin_family = AF_INET;
    writesoc.sin_port = htons(WPORT1);
    writesoc.sin_addr.s_addr = INADDR_ANY;
    //    inet_pton(AF_INET, ("127.0.0.1"), &dat.sin_addr.s_addr);
    if (bind(writer, (sockaddr*)&writesoc, sizeof(writesoc)) < 0 ){
        std::cerr << "[ERROR] Socket Bind Failed" << std::endl;
        close(reader);
        return 0;
    }
    else{
        std::cout << "[INFO] Socket bound" << std::endl;
    }
    if (listen(writer, 2) < 0){
        std::cerr << "[ERROR] Can't listen on socket" << std::endl;
        close(reader);
        return 0;
    }
    else{
        std::cout << "[INFO] Socket listening" << std::endl;
    }
    while (true) {
        int con = accept(reader, NULL, NULL);
        std::cout<<con<<std::endl;
        int writecon = accept(writer, NULL, NULL);
        std::cout << writecon << std::endl;
        // 2 threads for each client 
        if (con < 0){
            std::cerr << "[ERROR] Socket failed to accept connection" << std::endl;
            return -1;
        }
        else{
            std::cout << "[INFO] Socket Accepted connection from client" << std::endl;
        }
        sendlist.emplace_back(send_to_client,writecon);
        proc.emplace_back(process,con);
        sendlist.back().detach();
        proc.back().detach();

    }
    close(reader);
    close(writer);
}
