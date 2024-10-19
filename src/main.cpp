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
#include <cstring> // For strcmp
#include <sstream>

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

class Puck {
public:
    Color color;
    double x,y;
    int movx;
    int movy;
    int r;

    void drawcric() {
        DrawCircle(x, y, r, color);
    }

    void newpos() {
        if (y >= HEIGHT - r || y <= r) {
            movy *= -1;
        }
        x += movx;
        y += movy;
    }

    void reset() {
        x = WIDTH / 2;
        y = HEIGHT / 2;
    }
};

class striker {
public:
    Color color;
    int score;
    float width, height;
    float x, y;
    int mov;

    void draw() {
        DrawRectangle(x, y, width, height, color);
    }

    void newpos() {
        if (y <= 0) {
            y = 0;
        }
        if (y >= HEIGHT - height) {
            y = HEIGHT - height;
        }
        if (IsKeyDown(KEY_K)) {
            y -= mov;
            buf[1] = y;
        }
        if (IsKeyDown(KEY_J)) {
            y += mov;
            buf[1] = y;
        }
    }
};

void send_to_peer(int socket_fd, std::string peer_ip, float* buffer, SSL_CTX* ctx_sender, int receive_port) {
    sockaddr_in peer_addr;
    peer_addr.sin_family = AF_INET;
    inet_pton(AF_INET, peer_ip.c_str(), &peer_addr.sin_addr);
    peer_addr.sin_port = htons(receive_port); // Use configurable port
    SSL* sender_ssl = SSL_new(ctx_sender);
    int con = -1;
    while(con < 0) {
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
        SSL_write(sender_ssl, buffer, sizeof(float) * 2);
        usleep(000); 
    }
}

void receive_from_peer(int socket_fd, float* buffer, SSL_CTX* ctx_receiver, int receive_port) {
    sockaddr_in sender_addr;
    socklen_t sender_len = sizeof(sender_addr);
    sender_addr.sin_family = AF_INET;
    sender_addr.sin_port = htons(receive_port);
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
        SSL_read(receiver_ssl, buffer, sizeof(float) * 2);
        readbuf[1] = buffer[1];
        usleep(000); 
    }
}

// Function to parse command-line arguments
void parse_arguments(int argc, char* argv[], int& ball_speed, int& paddle_height, int& paddle_width, int& window_width, int& window_height, int& receive_port, int& send_port) {
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--ball-speed") == 0 && i + 1 < argc) {
            ball_speed = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--paddle-height") == 0 && i + 1 < argc) {
            paddle_height = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--paddle-width") == 0 && i + 1 < argc) {
            paddle_width = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--window-width") == 0 && i + 1 < argc) {
            window_width = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--window-height") == 0 && i + 1 < argc) {
            window_height = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--receive-port") == 0 && i + 1 < argc) {
            receive_port = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--send-port") == 0 && i + 1 < argc) {
            send_port = atoi(argv[++i]);
        }
    }
}

// Function to read configuration from a file
void read_config(const std::string& filename, int& ball_speed, int& paddle_height, int& paddle_width, int& window_width, int& window_height, int& receive_port, int& send_port) {
    std::ifstream config_file(filename);
    std::string line;

    while (std::getline(config_file, line)) {
        std::istringstream iss(line);
        std::string key;
        if (std::getline(iss, key, '=')) {
            std::string value;
            if (std::getline(iss, value)) {
                if (key == "ball_speed") ball_speed = std::stoi(value);
                else if (key == "paddle_height") paddle_height = std::stoi(value);
                else if (key == "paddle_width") paddle_width = std::stoi(value);
                else if (key == "window_width") window_width = std::stoi(value);
                else if (key == "window_height") window_height = std::stoi(value);
                else if (key == "receive_port") receive_port = std::stoi(value);
                else if (key == "send_port") send_port = std::stoi(value);
            }
        }
}

int main(int argc, char* argv[]) {
    SetTraceLogLevel(LOG_NONE);

    // Set default values
    int ball_speed = 10;
    int paddle_height = PADDLEH;
    int paddle_width = PADDLEW;
    int window_width = WIDTH;
    int window_height = HEIGHT;
    int receive_port = RPORT_RECEIVE;
    int send_port = RPORT_SEND;

    // Uncomment the next line to use the configuration file method instead
    // read_config("config.txt", ball_speed, paddle_height, paddle_width, window_width, window_height, receive_port, send_port);

    // Use command-line arguments if provided
    parse_arguments(argc, argv, ball_speed, paddle_height, paddle_width, window_width, window_height, receive_port, send_port);

    SSL_CTX* ctx_sender = initialize_ssl_context(false);
    SSL_CTX* ctx_receiver = initialize_ssl_context(true);
    int sender_socket = socket(AF_INET, SOCK_STREAM, 0);
    int receiver_socket = socket(AF_INET, SOCK_STREAM, 0);

    std::string peer_ip;
    std::cout << "Enter the other player's IP: ";
    std::cin >> peer_ip;

    // Sender connection
    channel.emplace_back(send_to_peer, sender_socket, peer_ip, buf, ctx_sender, send_port);
    channel.back().detach();

    // Receiver connection
    channel.emplace_back(receive_from_peer, receiver_socket, readbuf, ctx_receiver, receive_port);
    channel.back().detach();

    // Initialize game objects
    striker p1;
    p1.color = BLUE;
    p1.score = 0;
    p1.width = paddle_width;
    p1.height = paddle_height;
    p1.x = 0;
    p1.y = (HEIGHT - p1.height) / 2;
    p1.mov = 10;

    Puck ball;
    ball.color = RED;
    ball.x = WIDTH / 2;
    ball.y = HEIGHT / 2;
    ball.movx = ball_speed;
    ball.movy = ball_speed;
    ball.r = 10;

    InitWindow(window_width, window_height, "Multiplayer Pong");

    while (!WindowShouldClose()) {
        // Update player position
        p1.newpos();

        // Update ball position
        ball.newpos();

        // Check for collisions with paddles
        if (ball.x <= p1.x + p1.width && ball.y >= p1.y && ball.y <= p1.y + p1.height) {
            ball.movx *= -1;
            ball.x = p1.x + p1.width; // Prevent ball from getting stuck
        }

        // Check for scoring
        if (ball.x < 0) {
            ball.reset();
            ball.movx = ball_speed; // Reset ball speed
            ball.movy = ball_speed;
        } else if (ball.x > window_width) {
            ball.reset();
            ball.movx = -ball_speed; // Reset ball speed
            ball.movy = -ball_speed;
        }

        // Update buffer with player position
        buf[0] = p1.y;
        // Draw
        BeginDrawing();
        ClearBackground(RAYWHITE);
        ball.drawcric();
        p1.draw();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
