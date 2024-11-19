#ifndef BUFFER_H
#define BUFFER_H
#include <stdint.h> //declare specific size

typedef struct buffer_item {    //structure of buffer
    uint8_t data[30];
    uint16_t cksum;
} BUFFER_ITEM;

#define BUFFER_SIZE 10  //size of buffer

int insert_item(BUFFER_ITEM item);
int remove_item(BUFFER_ITEM *item);

#endif