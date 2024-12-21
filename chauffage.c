#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define MAX_BUFFER 1024
#define MULTICAST_GROUP "239.255.42.99"
#define MULTICAST_PORT 5000

int power_level = 0; // Niveau de chauffage (0 à 5)

/**
 * Fonction pour recevoir les messages multicast et ajuster la puissance.
 */
void *receive_multicast(void *arg) {
    int sock;
    struct sockaddr_in multicast_addr;
    struct ip_mreq mreq;
    char buffer[MAX_BUFFER];
    ssize_t n;

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("Socket");
        exit(1);
    }

    memset(&multicast_addr, 0, sizeof(multicast_addr));
    multicast_addr.sin_family = AF_INET;
    multicast_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    multicast_addr.sin_port = htons(MULTICAST_PORT);

    if (bind(sock, (struct sockaddr *)&multicast_addr, sizeof(multicast_addr)) < 0) {
        perror("Bind");
        exit(1);
    }

    mreq.imr_multiaddr.s_addr = inet_addr(MULTICAST_GROUP);
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);

    if (setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
        perror("Setsockopt");
        exit(1);
    }

    while (1) {
        n = recvfrom(sock, buffer, MAX_BUFFER, 0, NULL, 0);
        if (n > 0) {
            buffer[n] = '\0';
            printf("Message reçu : %s\n", buffer);

            int new_level = atoi(buffer);
            if (new_level >= 0 && new_level <= 5) {
                power_level = new_level;
                printf("Puissance de chauffage mise à jour : %d\n", power_level);
            }
        }
    }

    close(sock);
    return NULL;
}

int main() {
    pthread_t multicast_thread;

    if (pthread_create(&multicast_thread, NULL, receive_multicast, NULL) != 0) {
        perror("Thread");
        exit(1);
    }

    printf("Heater actif. En attente de messages multicast...\n");
    pthread_join(multicast_thread, NULL);

    return 0;
}
