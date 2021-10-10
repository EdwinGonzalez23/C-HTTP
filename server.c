/* C Libraries */
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h> /* Close */

#include "structs/http_handler.h"

#define PORT 8081
#define LISTEN_BACKLOG 50
#define BUFFER_SIZE 1000000

// FUNCTION PROTOTYPES
void check_initial_response_line(struct http_handler *hl);
char *returnLandingPage();

/*int argc, char *argv[]*/
int main() {

  int socket_file_desc, new_socket;
  long read_len;
  struct sockaddr_in address;
  int address_len = sizeof(address);

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

  char delim[] = "\n";
  char *response = returnLandingPage();
  while (1) {

    struct http_handler http_handler;
    int incoming_http_message_line_pos = 0;

    printf("WAITING ON NEW CONNECTION\n");

    if ((new_socket = accept(socket_file_desc, (struct sockaddr *)&address,
                             (socklen_t *)&address_len)) < 0) {
      perror("accept() error: See "
             "https://man7.org/linux/man-pages/man2/accept.2.html\n");
      exit(EXIT_FAILURE);
    }

    // TODO: Dynamic assignment
    char incoming_http_message[BUFFER_SIZE] = {0};
    read_len = read(new_socket, incoming_http_message, BUFFER_SIZE);
    printf("%s\n", incoming_http_message);

    /*
        READ INCOMING HTTP MESSAGE LINE BY LINE
    */
    char *incoming_http_message_ptr = strtok(incoming_http_message, delim);
    while (incoming_http_message_ptr != NULL) {
      switch (incoming_http_message_line_pos) {
      case 0:
        http_handler.initial_response_line = incoming_http_message_ptr;
        break;

      default:
        break;
      }

      incoming_http_message_ptr = strtok(NULL, delim);
      incoming_http_message_line_pos++;
    }

    /*
      CHECK CLIENT HEADERS
    */
    check_initial_response_line(&http_handler);

    // CHECK IF INITIAL HEADER IS SET
    int ver_num_offset =
        strlen(http_handler.http_ver) - 4; // Account for the /r: Ex HTTP/1.1\r
    char ver_num_char[3];
    strncpy(ver_num_char, &http_handler.http_ver[ver_num_offset],
            sizeof(ver_num_char));
    float ver_num = atof(ver_num_char);

    if (ver_num < 1.1) {
      printf("HTTP 1.0\n");
    } else if (ver_num > 1.0 && ver_num < 2.0) {
      printf("HTTP 1.x\n");
    } else if (ver_num >= 2.0 && ver_num < 3.0) {
      printf("HTTP 2.x\n");
    }

    /*
      CHECK REQUESTED RESOURCE PATH
    */
    if (strcmp("/", http_handler.requested_resource_file_path) == 0) {
      write(new_socket, response, strlen(response));
    } else {
      // MAKE THIS HTTP ERROR RESOURCE NOT FOUND
      write(new_socket, "Invalid Resource Request", strlen(response));
    }
    printf("RESPONSE SENT\n");

    close(new_socket);
  }
  close(socket_file_desc);

  return 0;
}

/*
  FUNCTIONS
*/

char *returnLandingPage() {
  FILE *fptr;
  fptr = fopen("index.html", "r");

  if (fptr == NULL) {
    perror("Failed to open G-HTTP landing page: See "
           "https://man7.org/linux/man-pages/man3/fopen.3.html");
    exit(EXIT_FAILURE);
  }

  char landing_page_buffer[BUFFER_SIZE + 1];
  if (fptr != NULL) {
    size_t new_len =
        fread(landing_page_buffer, sizeof(char), BUFFER_SIZE, fptr);
    if (ferror(fptr) != 0) {
      perror("Failed to read G-HTTP Landing Page into buffer.");
      exit(EXIT_FAILURE);
    } else {
      landing_page_buffer[new_len++] = '\0';
    }
  }
  fclose(fptr);

  /*
      Prep G-HTTP Landing page
  */

  // Prepare Headers
  int content_len = strlen(landing_page_buffer);
  char content_len_header[content_len];

  sprintf(content_len_header, "Content-Length: %d\n\n", content_len);

  char *response = "HTTP/1.1 200 OK\nContent-Type: text/html\n";

  int landing_page_len = strlen(response) + strlen(landing_page_buffer) +
                         strlen(content_len_header);
  char response_buff[landing_page_len + 1];

  // Combine HTTP Response text
  strcat(response_buff, response);
  strcat(response_buff, content_len_header);
  strcat(response_buff, landing_page_buffer);
  response_buff[landing_page_len++] = '\0';

  response = NULL;
  response = response_buff;
  return response;
}