# PongDaddy

A modern implementation of the classic Pong game with network multiplayer support and SSL encryption. Players can compete against each other over a local network with secure communication.

## Features

- Real-time multiplayer gameplay
- SSL/TLS encrypted communication
- Classic Pong mechanics
- Score tracking
- Clean, minimalist graphics

## Prerequisites

- C++ compiler with C++17 support
- CMake (version 3.15 or higher)
- Raylib (version 3.0 or higher)
- OpenSSL development libraries
- SSL certificates (for secure communication)

### System-specific Dependencies

#### Ubuntu/Debian
```bash
sudo apt install cmake build-essential libraylib-dev libssl-dev
```

#### macOS
```bash
brew install cmake raylib openssl
```

## Building

### Using CMake (Recommended)
```bash
mkdir build
cd build
cmake ..
make
```

### Using Make
```bash
make
```

Note: When using the Makefile, you can specify a custom Raylib path:
```bash
RAYLIB_PATH=/path/to/raylib make
```

## Running the Game

1. Generate SSL certificates (required for first-time setup):
```bash
openssl req -x509 -newkey rsa:4096 -keyout key.pem -out certificate.pem -days 365 -nodes
```

2. Start the game:
```bash
./pongdaddy
```

3. Enter the other player's IP address when prompted

## Controls

- `J` - Move paddle down
- `K` - Move paddle up
- `ESC` - Exit game

## Project Structure

```
pongdaddy/
├── src/
│   └── main.cpp
├── CMakeLists.txt
├── Makefile
├── certificate.pem
├── key.pem
└── README.md
```

## Security Notes

- The game uses SSL/TLS encryption for secure communication between players
- Ensure your SSL certificates are properly configured
- Keep your private key secure and never share it

## Known Issues

- SSL certificate paths are currently hardcoded and need to be updated manually
- Players must be on the same local network
- Only supports two players at a time

## Contributing

Feel free to open issues or submit pull requests with improvements!

## License

This project is open source. Feel free to use and modify as needed.
