#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

#define TCP_PORT 6000
#define MULTICAST_GROUP "239.255.42.99"
#define MULTICAST_PORT 5000

void *tcp_server(void *arg) {
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    char buffer[1024];
    socklen_t addr_len = sizeof(client_addr);

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("Socket");
        exit(1);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(TCP_PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind");
        exit(1);
    }

    if (listen(server_sock, 5) < 0) {
        perror("Listen");
        exit(1);
    }

    printf("Serveur TCP actif sur le port %d.\n", TCP_PORT);

    while (1) {
        client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &addr_len);
        if (client_sock < 0) {
            perror("Accept");
            continue;
        }

        memset(buffer, 0, sizeof(buffer));
        read(client_sock, buffer, sizeof(buffer) - 1);
        printf("Commande reçue : %s\n", buffer);
        // Traiter la commande ici
        close(client_sock);
    }

    close(server_sock);
    return NULL;
}

int main() {
    pthread_t tcp_thread;

    if (pthread_create(&tcp_thread, NULL, tcp_server, NULL) != 0) {
        perror("Thread TCP");
        exit(1);
    }

    pthread_join(tcp_thread, NULL);
    return 0;
}
