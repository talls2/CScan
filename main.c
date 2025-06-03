#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>

int scan_port(const char *target_ip, int port) {
    int sock;
    struct sockaddr_in server;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket creation failed");
        return -1;
    }

    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;

    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));
    setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout));

    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr(target_ip);

    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        close(sock);
        return 0;
    }

    close(sock);
    return 1;
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Usage: %s <target IP> <start port> <end port>\n", argv[0]);
        return 1;
    }

    char *target_ip = argv[1];
    int start_port = atoi(argv[2]);
    int end_port = atoi(argv[3]);

    if (start_port < 1 || end_port > 65535 || start_port > end_port) {
        printf("Invalid port range. Ports must be between 1 and 65535.\n");
        return 1;
    }

    printf("Scanning %s from port %d to %d...\n\n", target_ip, start_port, end_port);

    for (int port = start_port; port <= end_port; port++) {
        int result = scan_port(target_ip, port);
        if (result == 1) {
            printf("[+] Port %d is OPEN\n", port);
        } else {
            printf("[-] Port %d is closed\n", port);
        }
    }

    printf("\nScan complete.\n");
    return 0;
}
