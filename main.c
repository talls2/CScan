#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <pthread.h>

FILE *output_file;
pthread_mutex_t lock;

typedef struct {
    char scan_type[4];
    char target_ip[100];
    int port;
} scan_args;

int tcp_scan(const char *target_ip, int port) {
    int sock;
    struct sockaddr_in server;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        return -1;
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr(target_ip);

    struct timeval timeout;
    timeout.tv_sec = 2;
    timeout.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));

    int conn = connect(sock, (struct sockaddr *)&server, sizeof(server));

    close(sock);

    if (conn == 0) {
        return 1;
    } else {
        return 0;
    }
}

int udp_scan(const char *target_ip, int port) {
    int sock;
    struct sockaddr_in server;
    char buffer[1] = {0};

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        return -1;
    }

    struct timeval timeout;
    timeout.tv_sec = 2;
    timeout.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));

    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr(target_ip);

    int send_status = sendto(sock, buffer, sizeof(buffer), 0, (struct sockaddr*)&server, sizeof(server));
    if (send_status < 0) {
        close(sock);
        return -1;
    }

    char recv_buffer[1024];
    socklen_t server_len = sizeof(server);
    int recv_status = recvfrom(sock, recv_buffer, sizeof(recv_buffer), 0, (struct sockaddr*)&server, &server_len);

    close(sock);

    if (recv_status < 0) {
        return 1;
    } else {
        return 0;
    }
}

void *scan_thread(void *arguments) {
    scan_args *args = (scan_args *)arguments;
    int result;

    if (strcmp(args->scan_type, "tcp") == 0) {
        result = tcp_scan(args->target_ip, args->port);
        if (result == 1) {
            printf("[+] TCP Port %d is OPEN\n", args->port);
            pthread_mutex_lock(&lock);
            fprintf(output_file, "TCP Port %d OPEN\n", args->port);
            pthread_mutex_unlock(&lock);
        }
    } else if (strcmp(args->scan_type, "udp") == 0) {
        result = udp_scan(args->target_ip, args->port);
        if (result == 1) {
            printf("[?] UDP Port %d is OPEN|FILTERED\n", args->port);
            pthread_mutex_lock(&lock);
            fprintf(output_file, "UDP Port %d OPEN|FILTERED\n", args->port);
            pthread_mutex_unlock(&lock);
        }
    }

    free(args);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        printf("Usage: %s <tcp|udp> <target IP> <start port> <end port>\n", argv[0]);
        return 1;
    }

    char *scan_type = argv[1];
    const char *target_ip = argv[2];
    int start_port = atoi(argv[3]);
    int end_port = atoi(argv[4]);

    if (start_port < 1 || end_port > 65535 || start_port > end_port) {
        printf("Invalid port range.\n");
        return 1;
    }

    output_file = fopen("results.txt", "w");
    if (output_file == NULL) {
        printf("Failed to open results.txt for writing.\n");
        return 1;
    }

    pthread_mutex_init(&lock, NULL);

    printf("Starting %s scan on %s from port %d to %d...\n\n", scan_type, target_ip, start_port, end_port);

    pthread_t threads[65535];
    int thread_count = 0;

    for (int port = start_port; port <= end_port; port++) {
        scan_args *args = malloc(sizeof(scan_args));
        strcpy(args->scan_type, scan_type);
        strcpy(args->target_ip, target_ip);
        args->port = port;

        if (pthread_create(&threads[thread_count], NULL, scan_thread, (void *)args) != 0) {
            printf("Failed to create thread for port %d\n", port);
            free(args);
        }
        thread_count++;
        usleep(1000); // slight delay to prevent thread explosion
    }

    for (int i = 0; i < thread_count; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&lock);
    fclose(output_file);

    printf("\nScan complete. Open ports saved to results.txt\n");

    return 0;
}
