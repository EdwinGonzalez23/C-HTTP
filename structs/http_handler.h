#ifndef HTTP_HANDLER
#define HTTP_HANDLER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct http_handler {
  int x;

  char *initial_response_line;
  char *http_ver;
  char *method;
  char *requested_resource_file_path;
};

void check_initial_response_line(struct http_handler *h1) {

  char delim[] = " ";
  char resp_line[strlen(h1->initial_response_line)];

  strncpy(resp_line, h1->initial_response_line, sizeof(resp_line));

  char *ptr = strtok(resp_line, delim);
  // *ptr = NULL;

  int header_position = 0;
  while (ptr != NULL) {

    switch (header_position) {
    case 0:
      h1->method = ptr;
      break;
    case 1:
      h1->requested_resource_file_path = ptr;
      break;
    case 2:
      h1->http_ver = ptr;
      break;

    default:
      break;
    }
    ptr = strtok(NULL, delim);
    header_position++;
  }
}

#endif