/* C Libraries */
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h> /* Close */
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>

#define PORT 8080
#define LISTEN_BACKLOG 50
#define BUFFER_SIZE 1000000

/*int argc, char *argv[]*/
int main() {
    
    int socket_file_desc, new_socket;
    long read_len;
    struct sockaddr_in address;
    int address_len = sizeof(address);

    // Default G-HTTP landing page
    FILE *fptr;
    fptr = fopen("index.html", "r");

    if (fptr == NULL) {
        perror("Failed to open G-HTTP landing page: See https://man7.org/linux/man-pages/man3/fopen.3.html");
        exit(EXIT_FAILURE);
    }

    char g_land_buffer[BUFFER_SIZE + 1];
    if (fptr != NULL) {
        size_t new_len = fread(g_land_buffer, sizeof(char), BUFFER_SIZE, fptr);
        if (ferror(fptr) != 0) {
            perror("Failed to read G-HTTP Landing Page into buffer.");
            exit(EXIT_FAILURE);
        }
        else {
            g_land_buffer[new_len++] = '\0';
        }
    }
    fclose(fptr);

    /*
        Prep G-HTTP Landing page
    */

    //TODO: Get this working
    // int content_len_int = strlen(g_land_buffer);
    // char content_len[content_len_int];
    // sprintf(content_len, "Content-Length: %d\n\n", content_len_int);
    // printf("%s",content_len);

    char *response = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n";

    int g_land_resp_len = strlen(response) + strlen(g_land_buffer);
    char g_land_resp[g_land_resp_len + 1];

    strcat(g_land_resp, response);
    strcat(g_land_resp, g_land_buffer);
    g_land_resp[g_land_resp_len] = '\0';

    response = NULL;
    response = g_land_resp;

    /*
        AF_INET - IPv4, AF_UNSPEC - Allow IPv4 or IPv6
        SOCK_DGRAM, 0 - Both STREAM and DGRAM 
    */
    socket_file_desc = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_file_desc < 0) {
        perror("socket() error: See https://man7.org/linux/man-pages/man2/socket.2.html\n");
        exit(EXIT_FAILURE);
    }
    else {
        printf("Socket opened successfully.\n");
    }

    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
    address.sin_addr.s_addr = INADDR_ANY;

    memset(address.sin_zero, '\0', sizeof address.sin_zero);

    if (bind(socket_file_desc, (struct sockaddr *) &address, sizeof(address)) < 0) {
        perror("bind() error: See https://man7.org/linux/man-pages/man2/bind.2.html\n");
        exit(EXIT_FAILURE);
    }
    else {
        printf("Bind successful\n");
    }

    if (listen(socket_file_desc, LISTEN_BACKLOG) < 0) {
        perror("listen() error: See https://man7.org/linux/man-pages/man2/listen.2.html\n");
        exit(EXIT_FAILURE);
    }

    while (1) {
        printf("\n------------------WAITING ON NEW CONNECTION------------------------\n");
        if ((new_socket = accept(socket_file_desc, (struct sockaddr *) &address, (socklen_t*) &address_len)) < 0) {
            perror("accept() error: See https://man7.org/linux/man-pages/man2/accept.2.html\n");
            exit(EXIT_FAILURE);
        }
        char buffer[BUFFER_SIZE] = {0};
        read_len = read(new_socket, buffer, BUFFER_SIZE);
        printf("%s\n", buffer);
        write(new_socket, response, strlen(response));
        printf("\n------------------Response sent-------------------\n");
        close(new_socket);
    }
    close(socket_file_desc);

    return 0;
}