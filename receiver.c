#include "protocol.h"
#include <time.h>

#define RECV_WINDOW 10
#define LOSS_PROB 5 

void die(char *s)
 {
    perror(s);
    exit(1);
}

int should_drop() 
{
    return (rand() % 100) < LOSS_PROB;
}

int main() 
{
    init_sockets(); 

    struct sockaddr_in si_me, si_other;
    SOCKET s;
    int slen = sizeof(si_other);
    Packet recv_packet, ack_packet;
    
    uint32_t expected_seq = 0; 
    srand((unsigned int)time(NULL));

    if ((s = socket(AF_INET, SOCK_DGRAM, 0)) == -1) die("socket");

    memset(&si_me, 0, sizeof(si_me));
    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(SERVER_PORT);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(s, (struct sockaddr*)&si_me, sizeof(si_me)) == -1) die("bind");

    printf("Servidor (Windows) rodando na porta %d...\n", SERVER_PORT);

    while (1) 
    {
      
        if (recvfrom(s, (char*)&recv_packet, sizeof(Packet), 0, (struct sockaddr *) &si_other, &slen) == -1) die("recvfrom");

        if (recv_packet.header.flags & FLAG_DATA && should_drop())
         {
            printf("[DROP] Pacote Seq %d descartado.\n", recv_packet.header.seq_num);
            continue; 
        }

        if (recv_packet.header.flags & FLAG_SYN) 
        {
            printf("[RECV] SYN recebido.\n");
            ack_packet.header.flags = FLAG_SYN | FLAG_ACK;
            ack_packet.header.ack_num = 0;
            ack_packet.header.window_size = RECV_WINDOW;
            sendto(s, (char*)&ack_packet, sizeof(PacketHeader), 0, (struct sockaddr*) &si_other, slen);
            expected_seq = 0;
            continue;
        }

        if (recv_packet.header.flags & FLAG_DATA)
         {
            if (recv_packet.header.seq_num == expected_seq) 
            {
                xor_cipher(recv_packet.data, recv_packet.header.payload_len, INITIAL_KEY);
                printf("[RECV] Dado Seq %d Aceito.\n", expected_seq);
                expected_seq++;
            } else {
                printf("[RECV] Fora de ordem. Esperado: %d, Recebido: %d.\n", expected_seq, recv_packet.header.seq_num);
            }

            ack_packet.header.flags = FLAG_ACK;
            ack_packet.header.ack_num = expected_seq;
            ack_packet.header.window_size = RECV_WINDOW;
            
            sendto(s, (char*)&ack_packet, sizeof(PacketHeader), 0, (struct sockaddr*) &si_other, slen);
        }

        if (recv_packet.header.flags & FLAG_FIN) {
            printf("[FIN] Transferencia completa.\n");
            break;
        }
    }

    CLOSE_SOCKET(s);
    cleanup_sockets();
    return 0;
}