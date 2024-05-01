#include <stdio.h> // printf 
#include <stdlib.h> // malloc 
#include <stdint.h> 
#include <locale.h>
 
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
struct mem_header_t* mem_allocate(size_t mem_bytes);
void mem_free(struct mem_header_t* mem_block);

struct mem_header_t* start;

int main() {
    setlocale(LC_ALL, "Rus");
    start = malloc(32*MB); // ВСЕГО ПАМЯТИ НА "УСТРОЙСТВЕ"
    printf("Сведения: я подключил locale.h чисто для удобного мне вывода, чтоб свои комменты не писать на латинице\n"
    "так же есть варны в выводе хедера, но они связаны просто с выводом адресов\n\n");
    printf("Создана структура start, которая неиницилизирована, но выделенно под нее 32 мегабайта\n");
    print_header(start);
    printf("Инициализация структуры start и выделение под (виртуальную? хз как ее назвать) память на 40(структура 32 байта) байт\n");
    init_header(start, 8);
    print_header(start);
    printf("Создание нового блока памяти и выделение под него 42 байта\n");
    struct mem_header_t* mem_block2 = mem_allocate(12);
    print_header(mem_block2);
    printf("Создание еще одного блока:\n");
    struct mem_header_t* mem_block3 = mem_allocate(16);
    print_header(mem_block3);
    printf("Тем самым временем второй блок теперь указывает на третий блок(next header adr)\n");
    print_header(mem_block2);
    printf("Теперь сделаем освобождение памяти. Для этого сделаем еще два блока:\n");
    struct mem_header_t* mem_block4 = mem_allocate(34);
    print_header(mem_block4);
    printf("и\n");
    struct mem_header_t* mem_block5 = mem_allocate(50);
    print_header(mem_block5);
    printf("Теперь освободим память\n");
    mem_free(mem_block4);
    printf("После освобождения памяти посмотрим, как выглядит наш освобожденный блок под номером 4:\n");
    print_header(mem_block4);
    printf("Как видим, наш блок теперь ничего не весит(header size), не захвачен(is not free) и так же не на что не указывает.\n");
    printf("А так же посмотрим, как выглядит третий и четвертый блок:\n");
    print_header(mem_block3);
    printf("и\n");
    print_header(mem_block5);
    printf("Как видим, наш 3 и 5 блок связались, несмотря на то, что 4 блок был освобожден\n");
    printf("\n\nА теперь последний тест! Выделим два участка памяти, для того чтоб посмотреть:\n"
    "1. Займет ли первый блок память, на которой мы выделяли 4-ый блок\n"
    "2. Займет ли второй блок память, который будет после 5-ого блока\n");
    struct mem_header_t* mem_block6 = mem_allocate(100);
    struct mem_header_t* mem_block7 = mem_allocate(112);
    printf("И выведем их:\n");
    print_header(mem_block6);
    printf("и\n");
    print_header(mem_block7);
    printf("Как видим, наш шестой блок захватил блок, который до этого освобождался, а седьмой блок захватил участок, который был после пятого блока(последнего)\n");
    printf("Тесты закончены\n");
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

struct mem_header_t* mem_allocate(size_t mem_bytes) {
    struct mem_header_t* ret_adr = NULL;
    int i = 0;
    
    while(start[i+1].is_not_free != FALSE) i++;
    ret_adr = (struct mem_header_t*)(start+(i+1));
    

    init_header(ret_adr, mem_bytes);
    link_headers(ret_adr-1, ret_adr);

    return ret_adr;
}

void mem_free(struct mem_header_t* mem_block) {
    struct mem_header_t* prev_block = (struct mem_header_t*)mem_block->prev;
    struct mem_header_t* next_block = (struct mem_header_t*)mem_block->next;
    
    mem_block->size = 0;
    mem_block->is_not_free = FALSE;
    if(next_block != NULL) {
        prev_block->next = next_block;
        next_block->prev = prev_block;
    }
    mem_block->prev = NULL;
    mem_block->next = NULL;
}
