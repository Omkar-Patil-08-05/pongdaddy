cmake_minimum_required(VERSION 3.10)
project(pongdaddy)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenSSL REQUIRED)
find_package(Threads REQUIRED)

set(RAYLIB_INCLUDE_DIR "/usr/local/include")
set(RAYLIB_LIBRARY_DIR "/usr/local/lib")

add_executable(pongdaddy src/main.cpp)

target_include_directories(pongdaddy PRIVATE 
    ${RAYLIB_INCLUDE_DIR}
    ${OPENSSL_INCLUDE_DIR}
)

target_link_libraries(pongdaddy PRIVATE 
    ${OPENSSL_LIBRARIES}
    ${OPENSSL_SSL_LIBRARY}
    ${OPENSSL_CRYPTO_LIBRARY}
    raylib
    GL
    m
    pthread
    dl
    rt
    X11
)

if(UNIX AND NOT APPLE)
    target_link_libraries(pongdaddy PRIVATE atomic)
endif()

if(MSVC)
    target_compile_options(pongdaddy PRIVATE /W4)
else()
    target_compile_options(pongdaddy PRIVATE -Wall -Wextra)
endif()
