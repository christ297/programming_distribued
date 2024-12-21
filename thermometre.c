#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MULTICAST_GROUP "239.255.42.99"
#define MULTICAST_PORT 5000

float current_temperature = 20.0; // Température initiale

/**
 * Fonction pour simuler une mise à jour de la température.
 */
void update_temperature() {
    current_temperature += ((rand() % 10) - 5) * 0.1; // Variation aléatoire
    if (current_temperature < 10.0) current_temperature = 10.0;
    if (current_temperature > 30.0) current_temperature = 30.0;
}

/**
 * Fonction pour envoyer la température via multicast.
 */
void send_temperature(int sock, struct sockaddr_in *multicast_addr) {
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "%.2f", current_temperature);

    if (sendto(sock, buffer, strlen(buffer), 0,
               (struct sockaddr *)multicast_addr, sizeof(*multicast_addr)) < 0) {
        perror("Sendto");
    } else {
        printf("Température envoyée : %s\n", buffer);
    }
}

int main() {
    int sock;
    struct sockaddr_in multicast_addr;

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("Socket");
        exit(1);
    }

    memset(&multicast_addr, 0, sizeof(multicast_addr));
    multicast_addr.sin_family = AF_INET;
    multicast_addr.sin_addr.s_addr = inet_addr(MULTICAST_GROUP);
    multicast_addr.sin_port = htons(MULTICAST_PORT);

    printf("Thermomètre actif. Envoi des températures...\n");

    while (1) {
        update_temperature();
        send_temperature(sock, &multicast_addr);
        sleep(1); // Mise à jour toutes les secondes
    }

    close(sock);
    return 0;
}
