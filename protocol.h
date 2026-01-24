#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>


#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib") 
    
    #define CLOSE_SOCKET closesocket
    #define SLEEP_MS(x) Sleep(x)
#else
    #include <unistd.h>
    #include <arpa/inet.h>
    #include <sys/socket.h>
    #include <sys/time.h>
    #define CLOSE_SOCKET close
    #define SLEEP_MS(x) usleep((x)*1000)
    typedef int SOCKET;
#endif

#define MSS 1024
#define SERVER_PORT 8888
#define INITIAL_KEY 123



#define FLAG_SYN 0x01
#define FLAG_ACK 0x02
#define FLAG_FIN 0x04
#define FLAG_DATA 0x08

typedef struct
 {
    uint32_t seq_num;
    uint32_t ack_num;
    uint16_t window_size;
    uint8_t flags;
    uint8_t padding;
    uint16_t payload_len;
} PacketHeader;

typedef struct
 {
    PacketHeader header;
    char data[MSS];
} Packet;

void xor_cipher(char *data, int len, int key)
 {
    for (int i = 0; i < len; i++) {
        data[i] ^= key;
    }
}


void init_sockets()
 {
#ifdef _WIN32
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) 
    {
        printf("Falha ao iniciar Winsock. Codigo de erro: %d\n", WSAGetLastError());
        exit(1);
    }
#endif
}


void cleanup_sockets() 
{
#ifdef _WIN32
    WSACleanup();
#endif
}

#endif