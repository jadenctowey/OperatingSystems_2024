#ifndef FOPS_H
#define FOPS_H

#include <unistd.h>
#include <sys/stat.h>

int create_file(const char *filename, int flags, mode_t mode);
ssize_t write_to_file(int fd, const void *buf, size_t count);
ssize_t read_from_file(int fd, void *buf, size_t count);
int close_file(int fd);
int delete_file(const char *filename);

#endif // FOPS_H 