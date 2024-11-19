#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
int MAX_MEMORY; 


typedef struct memory_block {   // Structure to represent a memory block
    int start_address;
    int end_address;
    int size;
    char process[30];
    struct memory_block* next;  //points to next mem block
}memory_block;


memory_block** head;    //pointer to head (linked list)


void insert_before(memory_block* block_replace, int process_size, char process_name[30]){    //FUNC: insert new block before a specifc block
    printf("head is %s\n", (*head)->process);   //show process at head
    if((*head) == block_replace){        //if block replacing is head
        memory_block* block = malloc(sizeof(memory_block)); //creates a new block
        block->size = process_size; //size for the new block
        strcpy(block->process, process_name);   //copy process name

        block->start_address = block_replace->start_address;        //change address to start of block being replaced
        block->end_address = (block_replace->start_address - 1) + process_size;

        block_replace->start_address = block->end_address + 1;      //change the already replaced blocks address and size
        block_replace->size = block_replace->end_address - block_replace->start_address;

        block->next=(*head);    //new block in list
        *head = block;
    }
    else{       //for middle of the list, not if first block!
        memory_block* prev = NULL;     //mid list
        memory_block* curr = (*head);

        while (curr != block_replace) { //find the block to replace
            prev = curr;
            curr = curr->next;
        }

        memory_block* block = malloc(sizeof(memory_block));    // create a new block and initialize it
        block->size = process_size;
        strcpy(block->process, process_name);

        block->start_address = block_replace->start_address;        //for the address of new block
        block->end_address = (block_replace->start_address - 1) + process_size;

        block_replace->start_address = block->end_address + 1;      //for old block replace address and size
        block_replace->size = block_replace->end_address - (block_replace->start_address-1);

        block->next = prev->next;       //put new block into list
        prev->next = block;
    }
}


void block_stat(memory_block* block){   //to print stats for specific block
    if(strlen(block->process) == 0){
            printf("blockstat Addresses [%d:%d] Unused\n", block->start_address, block->end_address);
    }else{
        printf("blockstat Addresses [%d:%d] Process %s\n", block->start_address, block->end_address, block->process);
    }
}


void stat(){    //to print stats for all blocks
    memory_block* temp = (*head);
    while(temp != NULL){
        if(strlen(temp->process) == 0){
            printf("Addresses [%d:%d] Unused, size: %d\n", temp->start_address, temp->end_address, temp->size);
        }else{
            printf("Addresses [%d:%d] Process %s, size: %d\n", temp->start_address, temp->end_address, temp->process, temp->size);
        }
        temp = temp->next;
    }
}


void release(char process_name[30]){    //release memory used by specifc process
    printf("Releasing %s\n", process_name);
    memory_block* temp = (*head);
    while(temp != NULL){
        if(strcmp(temp->process, process_name) == 0){       //if block belongs to prcess
            memset(temp->process, '\0', sizeof(temp->process)); //clear process name
        } 
        temp = temp->next;
    }
    
}


void swap(memory_block* current, memory_block* prev){   //swaps the position of two elements in linked list 
    current->start_address += current->next->size;          // swap memory address of current and next
    current->end_address += current->next->size;
    current->next->start_address -= current->size;
    current->next->end_address -= current->size;

    if(prev != NULL){       //change pointers
        memory_block* temp = current->next->next;
        current->next->next = current;
        prev->next = current->next;
        current->next = temp;
    }
    else{                       //if the current block is the head
        (*head) = current->next;
        memory_block* temp = current->next->next;
        current->next->next = current;
        current->next = temp;
    }
}


void combine(memory_block* current){    //combine two unused memory blocks
    memory_block* temp = current->next->next;
    current->size += current->next->size;       //combine size
    current->end_address += current->next->size;    //change address 
    free(current->next);    //free next block
    current->next = temp;   //next block link
}

void compact() {    //compacts fragmented memory blocks - unused blocks to front of memory block (combine 1)
    memory_block* current = *head;
    memory_block* prev = NULL;

    while(current != NULL && current->next != NULL){    //move unused blocks to front
        if(strlen(current->process) == 0 && strlen(current->next->process) != 0){
            swap(current, prev);
            current= *head; //restart loop when swapped
            prev = NULL;
        }
        else{
            prev = current;
            current = current->next;
        }
    }

    current = *head;        //combine unused into 1 block
    while (current != NULL && current->next != NULL) {
        if (strlen(current->process) == 0 && strlen(current->next->process) == 0) {
            combine(current);
            current = (*head);
        }
        else{
            current = current->next;
        }
    }
}


void add_process(char process_name[30], int process_size, memory_block* block_replace){ //add process to the front of linked list 
    if(block_replace->size == process_size){ // if they are the same change the name without altering any blocks
        strcpy(block_replace->process, process_name);
        printf("same size");
    }
    else{
        insert_before(block_replace, process_size, process_name);   //if not then split block for the new process
    }
}


void add_first_process(char process_name[30], int process_size){    //add first process to mem
    memory_block* block = malloc(sizeof(struct memory_block));  //create new block
    block->size = process_size;
    block->start_address = 0;       //start beg. of mem
    block->end_address = process_size-1;
    strcpy(block->process, process_name);

    (*head)->size = MAX_MEMORY - process_size;      //adjust unused memory 
    (*head)->start_address = process_size;
    (*head)->end_address = MAX_MEMORY - 1;

    block->next = *head;        //new block at front of list 
    *head = block;
}


void firstFit(char process_name[30], int process_size){     //FIRST FIT (allocate mem)
    if((*head)->size == MAX_MEMORY){    //memory unused
        add_first_process(process_name, process_size);
    }
    else{
        memory_block* temp = *head;
        while(temp != NULL){
            if(temp->size >= process_size && strlen(temp->process) == 0){   //find the first block big enough for process
                add_process(process_name, process_size, temp);
                break;
            }
            temp = temp->next;
        }
    }
    stat(); //print mem stat
}


void bestFit(char process_name[30], int process_size){      //BEST FIT (allocate mem)
    int smallest_fitting_block = MAX_MEMORY;   //largest possible size init
    memory_block* temp = *head;
    memory_block* block_to_add = NULL;

    if((*head)->size == MAX_MEMORY){    //if mem is unused
        add_first_process(process_name, process_size);
    }
    else{
        while(temp != NULL){    //smallest block that can fit the process
            if(temp->size <= smallest_fitting_block && temp->size >= process_size && strlen(temp->process) == 0){
                smallest_fitting_block = temp->size;
                block_to_add = temp;
            }
            temp = temp->next; 
        }
        if(block_to_add != NULL){
            add_process(process_name, process_size, block_to_add);
        }
        else{
            printf("No block big enough to add process.\n");
        }
        stat(); //print stat of the mem
    }
}


void worstFit(char process_name[30], int process_size){     //WORST FIT (allocate mem)
    int largest_fitting_block = 0;  //smallest possible size init
    memory_block* temp = *head;
    memory_block* block_to_add = NULL;

    if((*head)->size == MAX_MEMORY){ // all mem is unused
        add_first_process(process_name, process_size);
    }
    else{
        while(temp != NULL){
            if(temp->size >= process_size && temp->size > largest_fitting_block && strlen(temp->process) == 0){ //find the largest block that fits process
                largest_fitting_block = temp->size;
                block_to_add = temp;
            }
            temp = temp->next; 
        }
        if(process_size <= block_to_add->size){
            add_process(process_name, process_size, block_to_add);
        }
        else{
            printf("No block is big enough to add process.\n");
        }
        stat(); //print stat of mem
    }
}

void allocate_memory(char process_name[30], int size, char strategy[2]){    //decide allocation strat
    if(strcmp(strategy, "F") == 0){ 
        printf("First Fit\n");
        firstFit(process_name, size);
    }
    else if(strcmp(strategy, "B") == 0){ 
        printf("Best Fit\n");
        bestFit(process_name, size);
    }
    else if(strcmp(strategy, "W") == 0){ 
        printf("Worst Fit\n");
        worstFit(process_name, size);
    }
}


int main(int argc, char* argv[]) {
    if (argc < 2) {     //if the user gave enough arguments
        printf("Error: Insufficient command-line arguments. Provide the max memory size.\n");
        return 1; 
    }

    MAX_MEMORY = atoi(argv[1]); //convert input to int

    if (MAX_MEMORY <= 0) {
        printf("Error: Invalid max memory size. Provide a positive integer.\n");
        return 2; 
    }

    head = malloc(sizeof(memory_block*));   //init mem w a single large block
    *head = malloc(sizeof(memory_block));

    (*head)->start_address = 0;
    (*head)->end_address = MAX_MEMORY - 1;
    (*head)->size = MAX_MEMORY;
    (*head)->process[0] = '\0'; // set process name to empty string
    (*head)->next = NULL;

    while (1) {     //handle user commands!
        char input[4];  //command type
        char process_name[30];  //process name
        int size;       //memory size
        char strategy[2];  //which allocation strat (best worst first)

        printf("allocator>");
        scanf("%s", input);

        if (strcmp(input, "RQ") == 0) {     //request mem
            scanf(" %s %d %s", process_name, &size, strategy);
            allocate_memory(process_name, size, strategy);
        }
        else if (strcmp(input, "RL") == 0) {    //release mem
            scanf(" %s", process_name);
            release(process_name);
        }
        else if (strcmp(input, "STAT") == 0) {      //show mem
            stat();
        }
        else if (strcmp(input, "C") == 0) {     //compact mem
            compact();
        }
        else if (strcmp(input, "X") == 0) { // exit command
            break;
        }
        else {
            printf("Error: Invalid input. Please enter RQ, RL, STAT, C, or X.\n");
        }
    }

    return 0;
}