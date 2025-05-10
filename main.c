#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int scan_port(const char *target_ip, int port) {
    int sock;
    struct sockaddr_in server;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket creation failed");
        return -1;
    }

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
    if(argc != 4){
        printf("Usage: %s <target IP> <start port> <end port> \n", argv[0]);
        return 1;
    }

    char *target_ip = argv[1];
    int start_port = atoi(argv[2]);
    int end_port = atoi(argv[3]);

     for (int port = start_port; port <= end_port; port++) {
        int result = scan_port(target_ip, port);
        if (result == 1) {
            printf("Port %d is OPEN\n", port);
        } else {
            printf("Port %d is closed\n", port);
        }
    }

    return 0;
}