#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "fops.h"
#define BUFFER_SIZE 256     //common used buffer size (looked one up)

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <operation> <args>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int fd = -1; 

    if (strcmp(argv[1], "create") == 0 && argc == 3) {      //strcmp -- compare strings 
        fd = create_file(argv[2], O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR);     //found flags needed in geeksforgeeks
        if (fd == -1) {
            return EXIT_FAILURE;    // error handling -- create_file
        }
        close_file(fd);     // close after creation
    } else if (strcmp(argv[1], "write") == 0 && argc == 4) {
        fd = create_file(argv[2], O_WRONLY | O_APPEND, 0);
        if (fd != -1) {
            if (write_to_file(fd, argv[3], strlen(argv[3])) == -1) {
                close_file(fd);
                return EXIT_FAILURE;    // error handling -- write_to_file
            }
            close_file(fd);             // close after write
        }
    } else if (strcmp(argv[1], "read") == 0 && argc == 3) {
        fd = create_file(argv[2], O_RDONLY, 0);
        if (fd != -1) {
            char buffer[BUFFER_SIZE];
            ssize_t bytes_read = read_from_file(fd, buffer, sizeof(buffer) - 1);
            if (bytes_read == -1) {
                close_file(fd);
                return EXIT_FAILURE; 
            }
            buffer[bytes_read] = '\0'; 
            printf("Read: %s\n", buffer);
            close_file(fd);             // close after read
        }
    } else if (strcmp(argv[1], "close") == 0 && argc == 3) {
        fprintf(stderr, "Close operation requires tracking file descriptor. Please implement this feature.\n");
    } else if (strcmp(argv[1], "delete") == 0 && argc == 3) {
        if (delete_file(argv[2]) == -1) {
            return EXIT_FAILURE; 
        }
    } else {
        fprintf(stderr, "Invalid operation or arguments.\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}