#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <netinet/ip.h>
#include <netinet/udp.h>

// Set the following port to a unique number:
#define MYPORT 5950

int create_cs3516_socket() {
    int sock;
    struct sockaddr_in server;
    
    sock = socket(AF_INET, SOCK_DGRAM, 0);

    if (sock < 0) error("Error creating CS3516 socket");

    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_ntop("10.63.38.2");
    server.sin_port = htons(MYPORT);
    if (bind(sock, (struct sockaddr *) &server, sizeof(server) ) < 0) 
        error("Unable to bind CS3516 socket");

    // Socket is now bound:
    return sock;
}

int cs3516_recv(int sock, char *buffer, int buff_size) {
    struct sockaddr_in from;
    int fromlen, n;
    fromlen = sizeof(struct sockaddr_in);
    n = recvfrom(sock, buffer, buff_size, 0,
                 (struct sockaddr *) &from, &fromlen);

    return n;
}

int cs3516_send(int sock, char *buffer, int buff_size, struct sockaddr_in to) {
    //struct sockaddr_in to;
    int tolen, n;

    //printf("%lu", nextIP);

    tolen = sizeof(struct sockaddr_in);

    // Okay, we must populate the to structure. 
    bzero(&to, sizeof(to));
    to.sin_family = AF_INET;
    to.sin_port = htons(MYPORT);
    //to.sin_addr.s_addr = nextIP;

    // We can now send to this destination:
    n = sendto(sock, buffer, buff_size, 0,
               (struct sockaddr *)&to, &tolen);

    printf("%d", n);

    return n;
}


int main() {
    char buffer[40];
    int sock = create_cs3516_socket();
    struct ip *ipHeader = (struct ip * ) buffer;
    struct udphdr *udpHeader = (struct udphdr *) (buffer+20);
    char *data = (buffer+20+8);

    

    uint32_t ipDest = inet_ntop("1.2.3.1");
    ipHeader->ip_dst.s_addr = ipDest;

    u_short udpSrc = 53;
    udpHeader->uh_sport = udpSrc;

    strncpy(data, "hello world", 12);
    //printf("NO DATA");
    
    struct sockaddr_in sa;
    inet_pton(AF_INET, "10.63.38.1", &(sa.sin_addr.s_addr));
    cs3516_send(sock, buffer, 40, sa);
    puts("BEFORE NO DATA");

    return sock;
}