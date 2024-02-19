#include <cstddef>
#include <iostream>
#include <arpa/inet.h>
#include <iterator>
#include <netinet/in.h>
#include <ostream>
#include <raylib.h>
#include <unistd.h>
#include <sys/socket.h>
 
#define PORT 8008
#define WIDTH 1000
#define HEIGHT 700
#define PADDLEH 100
#define PADDLEW 10

int main (int argc, char *argv[]) {
    InitWindow(WIDTH, HEIGHT, "Multiplayer Pong");
    SetTargetFPS(60);
    double y = 0;
    double mov = 1;
    while (!WindowShouldClose()) {
            if (IsKeyDown(KEY_K)) {
                y-=mov;
            }
            if (IsKeyDown(KEY_J)) {
                y+=mov;
            }
    }
    CloseWindow();
    return 0;
}

