#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if(argc != 4){
        printf("Usage: %s <target IP> <start port> <end port> \n", argv[0]);
        return 1;
    }

    char *target_ip = argv[1];
    int start_port = atoi(argv[2]);
    int end_port = atoi(argv[3]);

    printf("Scanning %s from port %d to %d\n", target_ip, start_port, end_port);

    return 0;
}