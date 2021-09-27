/* C Libraries */
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h> /* Close */

/*int argc, char *argv[]*/
int main() {
    
    int socket_file_desc;

    /*
        AF_INET - IPv4, AF_UNSPEC - Allow IPv4 or IPv6
        SOCK_DGRAM, 0 - Both STREAM and DGRAM 
    */
    socket_file_desc = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_file_desc == -1) {
        printf("socket() error: See https://man7.org/linux/man-pages/man2/socket.2.html\n");
    }
    else {
        printf("Socket opened successfully.\n");
    }

    close(socket_file_desc);

    return 0;
}