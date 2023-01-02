#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
//#include <arpa/inet.h>

#include <string>
#include <map>
#include <queue>
#include <iostream>

using namespace std;

// Set the following port to a unique number:
#define MYPORT 5950

struct node
{
    u_int32_t nextHopIP;
    bool isSet;
    struct node *zeroChild;
    struct node *oneChild;
};

void logger(char *x, char addr[])
{
	time_t t = time(NULL);

	struct tm tm = *localtime(&t);

	printf("Log Time: %s", ctime(&t));
	printf("Log Address: %s\n", addr);
	printf("Log Message: %s\n", x);
}

int create_cs3516_socket()
{
    int sock;
    struct sockaddr_in server;

    sock = socket(AF_INET, SOCK_DGRAM, 0);

    if (sock < 0)
        perror("Error creating CS3516 socket");

    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_ntop("10.63.38.1");
    server.sin_port = htons(MYPORT);
    if (bind(sock, (struct sockaddr *)&server, sizeof(server)) < 0)
        perror("Unable to bind CS3516 socket");

    // Socket is now bound:
    return sock;
    // return 0;
}

int cs3516_recv(int sock, char *buffer, int buff_size)
{
    puts("RECV");
    struct sockaddr_in from;
    int fromlen, n;
    fromlen = sizeof(struct sockaddr_in);
    n = recvfrom(sock, buffer, buff_size, 0,
                 (struct sockaddr *)&from, (socklen_t *)&fromlen);
    puts("RECV AFTER");

    return n;
    // return 0;
}

int cs3516_send(int sock, char *buffer, int buff_size, unsigned long nextIP)
{
    struct sockaddr_in to;
    int tolen, n;

    tolen = sizeof(struct sockaddr_in);

    // Okay, we must populate the to structure.
    bzero(&to, sizeof(to));
    to.sin_family = AF_INET;
    to.sin_port = htons(MYPORT);
    to.sin_addr.s_addr = nextIP;

    // We can now send to this destination:
    n = sendto(sock, buffer, buff_size, 0,
               (struct sockaddr *)&to, tolen);

    return n;
    // return 0;
}

int main()
{
    int sock = create_cs3516_socket();

    // reading config file
    FILE *ptr;
    char ch;

    // Opening file in reading mode
    ptr = fopen("config.txt", "r");

    if (NULL == ptr)
    {
        printf("file can't be opened \n");
    }

    printf("content of this file are \n");

    // Printing what is written in file
    // character by character using loop.

    int count = 0;
    char id = '0';
    int serverID = 0;
    int serverID2 = 0;
    string ipAddress = "";
    string overlayAddress = "";
    string delay1 = "";
    string delay2 = "";

    map<int, string> idToIP;
    map<string, string> ipToOverlay;
    map<int, string> idToDelaySend;
    map<int, string> idToDelayReceive;

    struct node *root = (struct node *)malloc(sizeof(struct node));

    root->zeroChild = NULL;
    root->oneChild = NULL;
    root->isSet = 0;
    root->nextHopIP = 0;

    queue<int> dropTailQueue;

    do
    {
        ch = fgetc(ptr);
        printf("%c", ch);

        if (ch == '\n')
        {
            if (id == '1')
            {
                idToIP.insert(pair<int, string>(id, ipAddress));
            }
            else if (id == '2')
            {
                idToIP.insert(pair<int, string>(id, ipAddress));
                ipToOverlay.insert(pair<string, string>(ipAddress, overlayAddress));
            }
            else if (id == '3')
            {
                idToDelaySend.insert(pair<int, string>(serverID, delay1));
                idToDelayReceive.insert(pair<int, string>(serverID2, delay2));
            }
            else if (id == '4')
            {
            }
            count = 0;
            id = '0';
            serverID = 0;
            serverID2 = 0;
            ipAddress = "";
            overlayAddress = "";
            delay1 = "";
            delay2 = "";
        }
        else if (ch == ' ')
        {
            count++;
        }
        else
        {
            if (count == 0)
            {
                if (ch == '1')
                {
                    id = 1;
                }
                else if (ch == '2')
                {
                    id = 2;
                }
                else if (ch == '3')
                {
                    id = 3;
                }
                else if (ch == '4')
                {
                    id = 4;
                }
            }

            if (count == 1)
            {
                serverID = ch - '0';
            }
            else if (count == 2 && (id == '1' || id == '2'))
            {
                ipAddress = ipAddress + ch;
            }
            else if (count == 3 && id == '2')
            {
                overlayAddress = overlayAddress + ch;
            }
            else if (count == 2 && id == '3')
            {
                delay1 = delay1 + ch;
            }
            else if (count == 3 && id == '3')
            {
                serverID2 = ch - '0';
            }
            else if (count == 4 && id == '3')
            {
                delay2 = delay2 + ch;
            }
        }

        // Checking if character is not EOF.
        // If it is EOF stop eading.
    } while (ch != EOF);

    // Closing the file
    fclose(ptr);

    // router
    while (1)
    {
        char dest_buffer[2000];
        int retval = cs3516_recv(sock, dest_buffer, 2000);
        struct ip *iphdr = (struct ip *)dest_buffer;
        struct udphdr *udp = (struct udphdr *)(dest_buffer + 20);
        char *data = (dest_buffer + 20 + 8);

        printf("DATA: %s", data);

        // time to live
        iphdr->ip_ttl--;
        // drop packet if 0
        if (iphdr->ip_ttl < 1)
        {
            logger("TTL Packet Dropped", "10.63.38.1");
            continue;
        }

        if (iphdr->ip_dst.s_addr == 5)
        {
            cs3516_send(sock, dest_buffer, 40, inet_pton("10.63.38.2"));
        }
        else
        {
            // send to Y
        }

        // drop tail queueing
        int delay = 100;
        logger("Drop Tail Queueing", "10.63.38.1");

        // longest prefix-matching and reading config filefor(int i = 0; i < 16; i++)

        // IP prefix: 1.2.3.4/16
        // 1.2.0.0
        // strtok() on / -> prefixLength
        int prefixLength = 16;

        struct node *currNode = root;

        for (int i = 0; i < prefixLength; i++)
        {
            // determine IP's bit #i - 0 or 1
            if (bit == 0)
            {
                // traverse and create the zeroChild
                if (currNode->zeroChild == NULL)
                {
                    // create
                    currNode->zeroChild = (struct node *)malloc(sizeof(struct node));
                    currNode->zeroChild->isSet = 0;
                    currNode->zeroChild->nextHopIP = 0;
                    currNode->zeroChild->zeroChild = 0;
                    currNode->zeroChild->oneChild = 0;

                    currNode = currNode->zeroChild;
                }
                else
                {
                    // traverse
                    currNode = currNode->zeroChild;
                    continue;
                }

                // finaly

                currNode->isSet = 1;
                // currNode->nextHopIP = actualNextHopIP;
            }
            else
            {
                // bit = 1
                // traverse and create the zeroChild
                if (currNode->oneChild == NULL)
                {
                    // create
                    currNode->oneChild = (struct node *)malloc(sizeof(struct node));
                    currNode->oneChild->isSet = 0;
                    currNode->oneChild->nextHopIP = 0;
                    currNode->oneChild->zeroChild = 0;
                    currNode->oneChild->oneChild = 0;

                    currNode = currNode->oneChild;
                }
                else
                {
                    // traverse
                    currNode = currNode->oneChild;
                    continue;
                }

                // finaly

                currNode->isSet = 1;
                // currNode->nextHopIP = actualNextHopIP;
            }
        }
    }

    logger("This project sucks", "10.63.38.1");
    return sock;
}