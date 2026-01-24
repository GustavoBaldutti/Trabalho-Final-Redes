#include "protocol.h"
#include <time.h>

#define TOTAL_PACKETS 1000 
#define INIT_SSTHRESH 64
#define TIMEOUT_MS 1000   

float cwnd = 1.0;
int ssthresh = INIT_SSTHRESH;
int rwnd = 10;
int dup_acks = 0;

FILE *log_file;

void log_status(double time_elapsed)
 {
    fprintf(log_file, "%f,%f,%d\n", time_elapsed, cwnd, ssthresh);
}

void congestion_control_success()
 {
    if (cwnd < ssthresh) cwnd += 1.0;
    else cwnd += 1.0 / cwnd;
}

void congestion_control_timeout()
 {
    ssthresh = (int)(cwnd / 2);
    if (ssthresh < 2) ssthresh = 2;
    cwnd = 1.0;
    printf("[TIMEOUT] Congestionamento! CWND: 1 SSTHRESH: %d\n", ssthresh);
}

int main()
 {
    init_sockets(); 

    struct sockaddr_in si_other;
    SOCKET s;
    int slen = sizeof(si_other);
    Packet packet, ack;
    
    log_file = fopen("congestion_log.csv", "w");
    fprintf(log_file, "time,cwnd,ssthresh\n");
    clock_t start_time = clock();

    if ((s = socket(AF_INET, SOCK_DGRAM, 0)) == -1) 
    {
        perror("socket");
        exit(1);
    }

 
    #ifdef _WIN32
        DWORD timeout = TIMEOUT_MS;
        setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));
    #else
        struct timeval tv;
        tv.tv_sec = 1; tv.tv_usec = 0;
        setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
    #endif

    memset(&si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(SERVER_PORT);
    

    si_other.sin_addr.s_addr = inet_addr("127.0.0.1");

   
    printf("Iniciando Handshake...\n");
    packet.header.flags = FLAG_SYN;
    packet.header.seq_num = 0;
    sendto(s, (char*)&packet, sizeof(PacketHeader), 0, (struct sockaddr *) &si_other, slen);

    if (recvfrom(s, (char*)&ack, sizeof(PacketHeader), 0, (struct sockaddr *) &si_other, &slen) != -1) {
        if (ack.header.flags & (FLAG_SYN | FLAG_ACK)) {
            printf("Conectado! Janela Remota: %d\n", ack.header.window_size);
            rwnd = ack.header.window_size;
        }
    } else {
        printf("Falha no handshake (timeout ou servidor desligado).\n");
        return 1;
    }


    int base = 0;
    int next_seq_num = 0;

    while (base < TOTAL_PACKETS) 
    {
        int effective_window = (int)cwnd < rwnd ? (int)cwnd : rwnd;

        while (next_seq_num < base + effective_window && next_seq_num < TOTAL_PACKETS) {
            packet.header.seq_num = next_seq_num;
            packet.header.flags = FLAG_DATA;
            packet.header.payload_len = MSS;
            memset(packet.data, 'A', MSS); 
            xor_cipher(packet.data, MSS, INITIAL_KEY);

            sendto(s, (char*)&packet, sizeof(PacketHeader) + MSS, 0, (struct sockaddr *) &si_other, slen);
            next_seq_num++;
        }

        if (recvfrom(s, (char*)&ack, sizeof(PacketHeader), 0, (struct sockaddr *) &si_other, &slen) != -1)
         {
            int ack_val = ack.header.ack_num;
            rwnd = ack.header.window_size;

            if (ack_val > base) 
            {
                base = ack_val;
                congestion_control_success();
                dup_acks = 0;
            } else
             {
                dup_acks++;
                if (dup_acks == 3) {
                     printf("[FAST RETRANSMIT] Seq %d\n", base);
                     ssthresh = (int)(cwnd / 2);
                     if (ssthresh < 2) ssthresh = 2;
                     cwnd = ssthresh + 3;
                     next_seq_num = base; 
                }
            }
        } else
         {
            congestion_control_timeout();
            next_seq_num = base;
        }

        double time_now = (double)(clock() - start_time) / CLOCKS_PER_SEC;
        log_status(time_now);
    }

    packet.header.flags = FLAG_FIN;
    sendto(s, (char*)&packet, sizeof(PacketHeader), 0, (struct sockaddr *) &si_other, slen);
    printf("Transferencia concluida.\n");
    
    fclose(log_file);
    CLOSE_SOCKET(s);
    cleanup_sockets();
    return 0;
}