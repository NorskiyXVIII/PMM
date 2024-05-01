#include <stdio.h> // printf 
#include <stdlib.h> // malloc 
#include <stdint.h> 
 
#define bool  int8_t 
#define KB    (1024) 
#define MB    (1024 * KB) 
#define TRUE  1 
#define FALSE 0 
 
struct mem_header_t { 
    size_t size; 
    bool is_not_free; 
     
    void* next; 
    void* prev; 
} mem_header_t; 

// прошлые функции
int init_header(struct mem_header_t* header, size_t mem_bytes);
void print_header(struct mem_header_t* header); 
int link_headers(struct mem_header_t* prev, struct mem_header_t* next); 
// новые функции
int destroy_header(struct mem_header_t* header);
struct mem_header_t* mem_allocate(struct mem_header_t* mem_block, size_t mem_bytes);
void mem_free(struct mem_header_t* mem_block);

int main() {
    struct mem_header_t* start = malloc(32 * MB);
    init_header(start, 2);
    print_header(start);
    struct mem_header_t* memory_block2 = mem_allocate(start, 4); 
    print_header(memory_block2);
    struct mem_header_t* memory_block3 = mem_allocate(memory_block2, 6); 
    print_header(memory_block3);
    struct mem_header_t* memory_block4 = mem_allocate(memory_block3, 8); 
    print_header(memory_block4);
    //mem_free(memory_block3);
    print_header(memory_block3);
    print_header(memory_block4);
    /*struct mem_header_t* memory_block2 = mem_allocate(memory_block1, 4); 
    struct mem_header_t* memory_block3 = mem_allocate(memory_block1, 8);
    struct mem_header_t* memory_block4 = mem_allocate(memory_block1, 12);
    struct mem_header_t* memory_block5 = mem_allocate(memory_block1, 16);
    print_header(memory_block1);
    print_header(memory_block2);
    print_header(memory_block3);
    print_header(memory_block4);
    print_header(memory_block5);
    mem_free(memory_block4);
    printf("\n");
    print_header(memory_block3);
    print_header(memory_block5);*/
    return 0;
}

int init_header(struct mem_header_t* header, size_t mem_bytes) { 
    header->size        = mem_bytes + sizeof(mem_header_t); 
    header->is_not_free = TRUE; 
    header->next        = NULL; 
    header->prev        = NULL; 
     
    return 0; 
} 

int link_headers(struct mem_header_t* prev, struct mem_header_t* next) { 
    prev->next = (void*)next; 
    next->prev = (void*)prev; 
    return 0; 
}

int destroy_header(struct mem_header_t* header) {
    header->size        = 0; 
    header->is_not_free = FALSE; 
    // разрушаем указатели на этот хедер и соеденяем предыдущий и следующий хедер
    struct mem_header_t* header_next = (struct mem_header_t*)header->next;
    struct mem_header_t* header_prev = (struct mem_header_t*)header->prev;
    if (header_next->prev != NULL) header_next->prev  = (struct mem_header_t*)header->prev;
    header_prev->next  = NULL;
    // разрушаем указатели этого хедера
    header->next  = (void*)NULL; 
    header->prev  = (void*)NULL; 
}


void print_header(struct mem_header_t* header) { 
    printf("-----------------\n");
    printf("Header size - %ld\n", header->size); 
    printf("Is not free? - %s\n", (header->is_not_free) ? "true" : "false"); 
    printf("Header adr - %u\n", header); 
    printf("\n"); 
    printf("Next header adr - %u\n", header->next); 
    //struct mem_header_t* header_prev = (struct mem_header_t*)header->prev;
    //printf("Prev header adr - %p {%ld}\n", header->prev, header_prev->size); 
    printf("Prev header adr - %u\n", header->prev); 
    printf("-----------------\n");
}

struct mem_header_t* mem_allocate(struct mem_header_t* mem_block, size_t mem_bytes) {
    struct mem_header_t* ret_adr = NULL;
    
    for (int i = 0; mem_block[i].is_not_free != FALSE; i++) {

        ret_adr = &mem_block[i+1];
        break;
    }
    
    if (ret_adr != NULL) {
        init_header(ret_adr, mem_bytes);
        link_headers(mem_block, ret_adr);
    }

    return ret_adr;
}

void mem_free(struct mem_header_t* mem_block) {
    struct mem_header_t* free_adr = NULL;
    
    for (int i = 0; free_adr[i].is_not_free != FALSE; i++) {

        free_adr = &mem_block[i+1];
        break;
    }
    
    if (free_adr != NULL) {
        link_headers(free_adr->prev, free_adr->next);
    }
}
