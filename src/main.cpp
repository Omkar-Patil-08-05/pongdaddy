#include <cstddef>
#include <fstream>
#include <ios>
#include <iostream>
#include <arpa/inet.h>
#include <iterator>
#include <netinet/in.h>
#include <ostream>
#include <raylib.h>
#include <string>
#include <unistd.h>
#include <sys/socket.h>
#include <vector>
#include <thread>
#include <openssl/ssl.h>
#include <openssl/err.h>
#define RPORT_RECEIVE 8089
#define RPORT_SEND 8008
#define WIDTH 1000
#define HEIGHT 700
#define PADDLEH 100
#define PADDLEW 10

bool con_stat = 0;

SSL_CTX* initialize_ssl_context(bool is_server) {
    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();

    SSL_CTX* ctx = SSL_CTX_new(is_server ? TLS_server_method() : TLS_client_method());
    if (!ctx) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    // Load server certificate and private key
    if (is_server) {
        if (SSL_CTX_use_certificate_file(ctx, "/home/vorrtt3x/cloned/pongdaddy/certificate.pem", SSL_FILETYPE_PEM) <= 0 ||
                SSL_CTX_use_PrivateKey_file(ctx, "/home/vorrtt3x/cloned/pongdaddy/key.pem", SSL_FILETYPE_PEM) <= 0) {
            ERR_print_errors_fp(stderr);
            exit(EXIT_FAILURE);
        }
    }

    return ctx;
}

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
            if (y>=HEIGHT-r ||y<=r ) {
                movy*=-1;
            }
            x+=movx;
            y+=movy;
        }
        void reset(){
            x = WIDTH/2;
            y = HEIGHT/2;
        }
};

class striker{
    public:
        Color color;
        int score;
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
void send_to_peer(int socket_fd,std::string peer_ip, float* buffer,SSL_CTX* ctx_sender) {
    sockaddr_in peer_addr;
    peer_addr.sin_family = AF_INET;
    inet_pton(AF_INET, peer_ip.c_str(), &peer_addr.sin_addr);
    peer_addr.sin_port = htons(RPORT_RECEIVE); // Port for receiving on the peer
    SSL* sender_ssl = SSL_new(ctx_sender);
    int con = -1;
    while(con<0){
        std::cout << "Trying to Connect" << std::endl;
        sleep(5);
        con = connect(socket_fd, (sockaddr*)&peer_addr, sizeof(peer_addr));
    }
    if (SSL_set_fd(sender_ssl, socket_fd) == 0) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
    if (SSL_connect(sender_ssl) <= 0) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    std::cout << "Connection Successful" << std::endl;
    con_stat = 1;
    while (true) {
        //        send(socket_fd, buffer, sizeof(float) * 2, 0);
        SSL_write(sender_ssl, buffer, sizeof(float)*2);
        usleep(000); 
    }
    
}

void receive_from_peer(int socket_fd, float* buffer,SSL_CTX* ctx_receiver) {
    sockaddr_in sender_addr;
    socklen_t sender_len = sizeof(sender_addr);
    sender_addr.sin_family = AF_INET;
    sender_addr.sin_port = htons(RPORT_RECEIVE);
    inet_pton(AF_INET, "192.168.1.7", &sender_addr.sin_addr);
    int bin = bind(socket_fd, (sockaddr*)&sender_addr, sender_len);
    listen(socket_fd, 1);
    int client_fd = accept(socket_fd, (sockaddr*)&sender_addr, &sender_len);
    SSL* receiver_ssl = SSL_new(ctx_receiver);
    if (!receiver_ssl) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
    if (SSL_set_fd(receiver_ssl, client_fd) == 0) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
    if (SSL_accept(receiver_ssl) <= 0) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
    while (true) {
//        recv(client_fd, buffer, sizeof(float) * 2, 0);
        SSL_read(receiver_ssl, buffer, sizeof(float)*2);
        readbuf[1] = buffer[1];
        usleep(000); 
    }
}
int main () {
    SetTraceLogLevel(LOG_NONE);
    SSL_CTX* ctx_sender = initialize_ssl_context(false);
    SSL_CTX* ctx_receiver = initialize_ssl_context(true);
    int sender_socket = socket(AF_INET, SOCK_STREAM, 0);
    int receiver_socket = socket(AF_INET, SOCK_STREAM, 0);

    std:: string peer_ip;
    std::cout << "Enter the other player's IP: ";
    std::cin >> peer_ip;

    // Sender connection
    channel.emplace_back(send_to_peer, sender_socket, peer_ip ,buf,ctx_sender);
    channel.back().detach();

    // Receiver connection
    channel.emplace_back(receive_from_peer, receiver_socket, readbuf,ctx_receiver);
    channel.back().detach();
    std::cout << "[INFO] Starting Game" << std::endl;
    Puck ball;
    striker p1;
    striker p2;
    InitWindow(WIDTH, HEIGHT, "Multiplayer Pong");
    SetTargetFPS(60);

    p1.mov = 10;
    p1.y = HEIGHT/2;
    p1.x = 10;
    p1.score = 0;
    p1.color = RAYWHITE;
    p1.width = PADDLEW;
    p1.height = PADDLEH;

    p2.mov = 10;
    p2.y = HEIGHT/2;
    p2.x = WIDTH-20;
    p2.score = 0;
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

        DrawLine(WIDTH/2, 0, WIDTH/2, HEIGHT, RAYWHITE);
        if (CheckCollisionCircleRec(Vector2{(float)ball.x,(float)ball.y}, ball.r, Rectangle{p1.x,p1.y,p1.width,p1.height})) {
            ball.movx*=-1;
        }
        p2.y = readbuf[1];

        if (ball.x+ball.r<=0) {
            p2.score++;
            ball.reset();
        }
        if (ball.x+ball.r>=WIDTH) {
            p1.score++;
            ball.reset();
        }
        p1.newpos();
        p2.draw();
        p1.draw();
        if (con_stat) {
            ball.newpos();
            ball.drawcric();
            DrawText(TextFormat("%i",p1.score), WIDTH/4-20, 20, 80, RAYWHITE);
            DrawText(TextFormat("%i",p2.score),3* WIDTH/4-20, 20, 80, RAYWHITE);
        }
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


