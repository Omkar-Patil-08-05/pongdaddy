#ifndef CLIENT
#define CLIENT
#include <netinet/in.h>
typedef struct cons{
    int readcon;
    int writecon;
}CONS;
void send_to_server(int cfd,float buf[2]);
void configure_sockets(sockaddr_in *writesoc, sockaddr_in *readsoc);
void receive_from_server(int reader, float readbuf[2]);
CONS create_sockets(int* writer, int* reader);
#endif // !CLIENT

