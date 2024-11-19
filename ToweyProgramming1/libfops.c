#include <sys/syscall.h>
#include <fcntl.h>  //note: control over open files (defines constants related to file control operations)
#include <stdio.h>
#include "fops.h"

int create_file(const char* filename, int flags, mode_t mode){   //mode is used because the file does not already exist
    int fd = open(filename, flags, mode);
    //int fd = syscall(SYS_open, filename, flags, mode);
    if (fd == -1) {
        perror("Error creating a file");
    }
    return fd;
} 

ssize_t write_to_file(int fd, const void *buf, size_t count){
    ssize_t results = write(fd, buf, count);
    //ssize_t results = syscall(SYS_write, fd, buf, count);
    if (results == -1){
        perror("Error writing to a file");
    }
    return results;
}

ssize_t read_from_file(int fd, void *buf, size_t count){
    ssize_t results = read(fd, buf, count);
   // ssize_t results = syscall(SYS_read, fd, buf, count);
    if (results == -1){
        perror("Error reading from a file");
    }
    return results;
}

int close_file(int fd){
    int results = close(fd);
    //int results = syscall(SYS_close, fd);
    if (results == -1){
        perror("Error closing a file");
    }
    return results;
}

int delete_file(const char *filename){
    int results = unlink(filename);
    //int results = syscall(SYS_unlink, filename);
    if (results == -1){
        perror("Error deleting a file");
    }
    return results;
}

//changed the SYS_   because i was getting a lot of warnings with them while compiling