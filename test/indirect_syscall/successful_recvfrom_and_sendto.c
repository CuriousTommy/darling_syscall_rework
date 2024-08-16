#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

extern int darling_syscall(int, ...);
#ifdef __APPLE__
extern int syscall(int,...);
#endif

extern void darling_set_cerror(int err);
extern int darling_get_cerror();

void* server_thread(void* args);
void* client_thread(void* args);

#define PORT 8080
#define MAXLINE 1024

int main() {
    pthread_t threads[2];
    if (pthread_create(&threads[0], NULL, server_thread,  NULL)) {
        perror("Failed to create server thread");
        exit(1);
    }

    if (pthread_create(&threads[1], NULL, client_thread,  NULL)) {
        perror("Failed to create client thread");
        exit(1);
    }

    pthread_join(threads[0], NULL);
    pthread_join(threads[1], NULL);
}

//
// The server and client implementation are based on this tutorial:
// https://www.geeksforgeeks.org/udp-server-client-implementation-c/
//

void* server_thread(void* args) {
    printf("In server_thread()\n");
    darling_set_cerror(0);

    int server_fd;
    if ((server_fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
        perror("Failed to create socket");
        exit(1);
    }

    struct sockaddr_in server_address_details = {0};
    server_address_details.sin_family = PF_INET;
    server_address_details.sin_addr.s_addr = INADDR_ANY;
    server_address_details.sin_port = htons(PORT);

    if (bind(server_fd, (const struct sockaddr*)&server_address_details,
            sizeof(server_address_details)) < 0) {
        perror("Failed to bind socket");
        exit(1);
    }

    assert(darling_get_cerror() == 0);
    struct sockaddr_in client_address_details = {0};
    socklen_t len = sizeof(client_address_details);
    char buffer[MAXLINE];
    printf("[Check pointer location] len& = %p\n", &len);
    // int client_buffer_len = recvfrom(server_fd, buffer, MAXLINE, MSG_WAITALL,
    //     (struct sockaddr*)&client_address_details, &len);
    int client_buffer_len = darling_syscall(29, server_fd, buffer, MAXLINE, MSG_WAITALL,
        (struct sockaddr*)&client_address_details, &len);
    assert(darling_get_cerror() == 0);
    assert(client_buffer_len > 0);

    buffer[client_buffer_len] = '\0';
    const char* expected_client_message = "[Client] Sent message to server";
    assert(strcmp(expected_client_message, buffer) == 0);

    const char* server_message = "[Server] Sent message to client";
    if (sendto(server_fd, server_message, strlen(server_message), 0,
            (const struct sockaddr*)&client_address_details, len) < 0) {
        perror("Failed to send message");
        exit(1);
    }

    assert(darling_get_cerror() == 0);
    return NULL;
}

void* client_thread(void* args) {
    printf("In client_thread()\n");
    darling_set_cerror(0);

    int client_fd;
    if ((client_fd = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Failed to create socket");
        exit(1);
    }

    struct sockaddr_in server_address_details = {0};
    server_address_details.sin_family = PF_INET;
    server_address_details.sin_addr.s_addr = INADDR_ANY;
    server_address_details.sin_port = htons(PORT);

    const char* client_message = "[Client] Sent message to server";
    sendto(client_fd, client_message, strlen(client_message), 0,
        (const struct sockaddr*)&server_address_details, sizeof(server_address_details));
    
    char buffer[MAXLINE];
    socklen_t len = sizeof(server_address_details);
    int server_buffer_len = recvfrom(client_fd, (char*)buffer, strlen(buffer), 0,
        (struct sockaddr*)&server_address_details, &len);
    
    buffer[server_buffer_len] = '\0';
    assert(strcmp("[Server] Sent message to client", buffer));

    close(client_fd);
    assert(darling_get_cerror() == 0);
    return NULL;
}