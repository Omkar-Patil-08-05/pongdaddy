#include <cstddef>
#include <fstream>
#include "clients.h"
#include <ios>
#include <iostream>
#include <arpa/inet.h>
#include <iterator>
#include <netinet/in.h>
#include <ostream>
#include <raylib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <vector>
#include <thread>
#define RPORT_RECEIVE 8089
#define RPORT_SEND 8008
#define WIDTH 1000
#define HEIGHT 700
#define PADDLEH 100
#define PADDLEW 10
//#define PIPE "/tmp/pongdaddy"
//
//

std::vector<std::thread> channel;
float buf[2] = {0,0};
float readbuf[2] = {-1,-1};
class Puck{
    public:
        Color color;
        double x,y;
        int movx;
        int movy;
        int r;
        void drawcric(){
            DrawCircle(x, y, r, color);
        }
        void newpos(){
            if (x>=WIDTH-r) {
                movx*=-1;
            }
            if (y>=HEIGHT-r ||y<=r ) {
                movy*=-1;
            }
            x+=movx;
            y+=movy;
        }
};

class striker{
    public:
        Color color;
        float width,height;
        float x, y;
        int mov;
//        void broadcast(float y){
//            std::ofstream pipe(PIPE,std::ios::out | std::ios::binary);
//            if (pipe.is_open()) {
//                pipe.write((const char*)(&y), sizeof(y));
//                pipe.close();
//            }
//        }
        void draw(){
            DrawRectangle(x, y, width, height, color);
        }
        void newpos(){
            if (y<=0) {
                y = 0;
            }
            if (y>=HEIGHT-height) {
                y = HEIGHT-height;
            }
            if (IsKeyDown(KEY_K)) {
                y-=mov;
                buf[1] = y;
//                broadcast(y);
            }
            if (IsKeyDown(KEY_J)) {
                y+=mov;
                buf[1] = y;
//                broadcast(y);
            }
        }
};
void send_to_peer(int socket_fd, float* buffer) {
    sockaddr_in peer_addr;
    peer_addr.sin_family = AF_INET;
    inet_pton(AF_INET, "192.168.1.96", &peer_addr.sin_addr);
    peer_addr.sin_port = htons(RPORT_RECEIVE); // Port for receiving on the peer
    int con = connect(socket_fd, (sockaddr*)&peer_addr, sizeof(peer_addr));
    if (con<0) {
        std::cout << "Failed to COnnect" << std::endl;
        return;
    }

    while (true) {
        send(socket_fd, buffer, sizeof(float) * 2, 0);
        usleep(1000); 
    }
}

void receive_from_peer(int socket_fd, float* buffer) {
    sockaddr_in sender_addr;
    socklen_t sender_len = sizeof(sender_addr);
    sender_addr.sin_family = AF_INET;
    sender_addr.sin_port = htons(RPORT_RECEIVE);
    inet_pton(AF_INET, "192.168.1.7", &sender_addr.sin_addr);
    int bin = bind(socket_fd, (sockaddr*)&sender_addr, sender_len);
    listen(socket_fd, 1);
    int client_fd = accept(socket_fd, (sockaddr*)&sender_addr, &sender_len);

    while (true) {
        recv(client_fd, buffer, sizeof(float) * 2, 0);
        readbuf[1] = buffer[1];
        usleep(9000); 
    }
}
int main () {
    SetTraceLogLevel(LOG_NONE);
    int sender_socket = socket(AF_INET, SOCK_STREAM, 0);
    int receiver_socket = socket(AF_INET, SOCK_STREAM, 0);

    // Sender connection
    channel.emplace_back(send_to_peer, sender_socket, buf);
    channel.back().detach();

    // Receiver connection
    channel.emplace_back(receive_from_peer, receiver_socket, readbuf);
    channel.back().detach();
    std::cout << "[INFO] Starting Game" << std::endl;
    Puck ball;
    striker p1;
    striker p2;
    InitWindow(WIDTH, HEIGHT, "Multiplayer Pong");
    std::cout << "[INFO] Starting Game" << std::endl;
    SetTargetFPS(60);

    p1.mov = 10;
    p1.y = HEIGHT/2;
    p1.x = 10;
    p1.color = RAYWHITE;
    p1.width = PADDLEW;
    p1.height = PADDLEH;

    p2.mov = 10;
    p2.y = HEIGHT/2;
    p2.x = WIDTH-20;
    p2.color = RAYWHITE;
    p2.width = PADDLEW;
    p2.height = PADDLEH;

    ball.color = RAYWHITE;
    ball.r = 20;
    ball.x = HEIGHT/2;
    ball.y = WIDTH/2;
    ball.movx = 10;
    ball.movy = 10;
    while (!WindowShouldClose()) {
        BeginDrawing();

        if (CheckCollisionCircleRec(Vector2{(float)ball.x,(float)ball.y}, ball.r, Rectangle{p1.x,p1.y,p1.width,p1.height})) {
            ball.movx*=-1;
        }
        p2.y = readbuf[1];

        p1.newpos();
        p2.draw();
        p1.draw();
        ball.newpos();
        ball.drawcric();
        Color color = {51, 51, 51, 255};
       // DrawRectangle(10, HEIGHT/2-PADDLEH/2, PADDLEW, PADDLEH, RAYWHITE);
       // DrawRectangle(WIDTH-15, HEIGHT/2-PADDLEH/2, PADDLEW, PADDLEH, RAYWHITE);
        ClearBackground(color);
        EndDrawing();
    }
    CloseWindow();
    close(sender_socket);
    close(receiver_socket);
    return 0;
}
