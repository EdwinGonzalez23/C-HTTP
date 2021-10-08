/* C Libraries */
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h> /* Close */

#include "structs/http_handler.h"

#define PORT 8080
#define LISTEN_BACKLOG 50
#define BUFFER_SIZE 1000000

// FUNCTION PROTOTYPES
void check_initial_response_line(struct http_handler *hl);

/*int argc, char *argv[]*/
int main() {

  // TEST STRUCT
  struct http_handler http_handler;
  http_handler.initial_response_line = "GET / HTTP/1.1";
  check_initial_response_line(&http_handler);

  // CHECK IF INITIAL HEADER IS SET
  int ver_num_offset = strlen(http_handler.http_ver) - 3;
  char ver_num_char[3];
  strncpy(ver_num_char, &http_handler.http_ver[ver_num_offset],
          sizeof(ver_num_char));
  float ver_num = atof(ver_num_char);

  if (ver_num < 1.1) {
    printf("HTTP 1.0");
  } else if (ver_num > 1.0 && ver_num < 2.0) {
    printf("HTTP 1.x");
  } else if (ver_num >= 2.0 && ver_num < 3.0) {
    printf("HTTP 2.x");
  }

  return 0;

  int socket_file_desc, new_socket;
  long read_len;
  struct sockaddr_in address;
  int address_len = sizeof(address);

  // Default G-HTTP landing page
  FILE *fptr;
  fptr = fopen("index.html", "r");

  if (fptr == NULL) {
    perror("Failed to open G-HTTP landing page: See "
           "https://man7.org/linux/man-pages/man3/fopen.3.html");
    exit(EXIT_FAILURE);
  }

  char g_land_buffer[BUFFER_SIZE + 1];
  if (fptr != NULL) {
    size_t new_len = fread(g_land_buffer, sizeof(char), BUFFER_SIZE, fptr);
    if (ferror(fptr) != 0) {
      perror("Failed to read G-HTTP Landing Page into buffer.");
      exit(EXIT_FAILURE);
    } else {
      g_land_buffer[new_len++] = '\0';
    }
  }
  fclose(fptr);

  /*
      Prep G-HTTP Landing page
  */

  // Prepare Headers
  int content_len = strlen(g_land_buffer);
  char content_len_header[content_len];

  sprintf(content_len_header, "Content-Length: %d\n\n", content_len);

  char *response = "HTTP/1.1 200 OK\nContent-Type: text/html\n";

  int g_land_resp_len =
      strlen(response) + strlen(g_land_buffer) + strlen(content_len_header);
  char g_land_resp[g_land_resp_len + 1];

  // Combine HTTP Response text
  strcat(g_land_resp, response);
  strcat(g_land_resp, content_len_header);
  strcat(g_land_resp, g_land_buffer);
  g_land_resp[g_land_resp_len++] = '\0';

  response = NULL;
  response = g_land_resp;

  /*
      AF_INET - IPv4, AF_UNSPEC - Allow IPv4 or IPv6
      SOCK_DGRAM, 0 - Both STREAM and DGRAM
  */
  socket_file_desc = socket(AF_INET, SOCK_STREAM, 0);

  if (socket_file_desc < 0) {
    perror("socket() error: See "
           "https://man7.org/linux/man-pages/man2/socket.2.html\n");
    exit(EXIT_FAILURE);
  } else {
    printf("Socket opened successfully.\n");
  }

  address.sin_family = AF_INET;
  address.sin_port = htons(PORT);
  address.sin_addr.s_addr = INADDR_ANY;

  memset(address.sin_zero, '\0', sizeof address.sin_zero);

  if (bind(socket_file_desc, (struct sockaddr *)&address, sizeof(address)) <
      0) {
    perror("bind() error: See "
           "https://man7.org/linux/man-pages/man2/bind.2.html\n");
    exit(EXIT_FAILURE);
  } else {
    printf("Bind successful\n");
  }

  if (listen(socket_file_desc, LISTEN_BACKLOG) < 0) {
    perror("listen() error: See "
           "https://man7.org/linux/man-pages/man2/listen.2.html\n");
    exit(EXIT_FAILURE);
  }

  while (1) {
    printf("WAITING ON NEW CONNECTION\n");

    if ((new_socket = accept(socket_file_desc, (struct sockaddr *)&address,
                             (socklen_t *)&address_len)) < 0) {
      perror("accept() error: See "
             "https://man7.org/linux/man-pages/man2/accept.2.html\n");
      exit(EXIT_FAILURE);
    }

    char incoming_connection_buffer[BUFFER_SIZE] = {0};
    read_len = read(new_socket, incoming_connection_buffer, BUFFER_SIZE);
    printf("%s\n", incoming_connection_buffer);

    /*
        CHECK CLIENT HEADERS
    */

    write(new_socket, response, strlen(response));
    printf("RESPONSE SENT\n");

    close(new_socket);
  }
  close(socket_file_desc);

  return 0;
}