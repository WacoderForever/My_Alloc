#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#define HEAP_CAPACITY 640000
#define CHUNK_CAPACITY 1024

#define UNIMPLEMENTED\
    do{\
        fprintf(stderr,"%s: %d: %s not implemented",\
                __FILE__,__LINE__,__func__);\
        abort();\
    } while(0)

char heap[HEAP_CAPACITY]={0};

typedef struct{

    void *start;
    size_t size;

}Chunk;

typedef struct{

    size_t count;
    Chunk chunks[CHUNK_CAPACITY];

}Chunk_List;

Chunk_List allocated_chunks={0};

Chunk_List free_chunks={

    .count=1,
    .chunks={
        [0]={.size=sizeof(heap),.start=heap}
    }

};

Chunk_List temp_chunks={0};

void chunk_list_dump(const Chunk_List *list){

    for(int i=0;i<list->count;i++){

        printf("start: %p size: %zu\n",
        list->chunks[i].start,list->chunks[i].size);
    }
    puts(" ");
}

void chunk_list_insert(Chunk_List *list,void *ptr,size_t size){

    list->chunks[list->count].start=ptr;
    list->chunks[list->count].size=size;

    for(int i=list->count;
    i>0 && list->chunks[i].start < list->chunks[i-1].start;
    i--){

        const Chunk t=list->chunks[i];
        list->chunks[i]=list->chunks[i-1];
        list->chunks[i-1]=t;
    }
    list->count++;

}

size_t chunk_list_find(const Chunk_List *list,void *ptr){

    for(int i=0;i<list->count;i++){

        if(list->chunks[i].start == ptr){

            return (size_t)i;
        }
    }

    return -1;
}

void chunk_list_remove(Chunk_List *list,size_t index){

    assert(list->count > index);

    for(int i=index;i<list->count-1;i++){
        
        list->chunks[i]=list->chunks[i+1];
    }

    list->count--;
}

void chunk_list_merge(Chunk_List *dest,Chunk_List *src){

    dest->count=0;

    for(int i=0; i<src->count ;i++){

        const Chunk chunk=src->chunks[i];

        if(dest->count > 0){

            Chunk *top_chunk=&dest->chunks[i-1];

            if(top_chunk->start + top_chunk->size == chunk.start){

                top_chunk->size += chunk.size;
            }

            else{

                chunk_list_insert(dest,chunk.start,chunk.size);
            }
        }
        else{

            chunk_list_insert(dest,chunk.start,chunk.size);
        }
    }
}

void *heap_alloc(size_t size){

    if(size >0){

        chunk_list_merge(&temp_chunks,&free_chunks);
        free_chunks=temp_chunks;
        
        for(int i=0 ; i<free_chunks.count;i++){

            Chunk chunk=free_chunks.chunks[i];
            void *ptr=chunk.start;

            chunk_list_remove(&free_chunks,i);

            size_t tailsize=chunk.size-size;

            chunk_list_insert(&allocated_chunks,ptr,size);

            if(tailsize > 0){

                chunk_list_insert(&free_chunks,chunk.start+size,tailsize);
            }

            return ptr;
        }
    }
}

void heap_free(void *ptr){

    if(ptr != NULL){

        const size_t index=chunk_list_find(&allocated_chunks,ptr);

        assert(index>=0);
        assert(ptr==allocated_chunks.chunks[index].start);

        chunk_list_insert(&free_chunks,allocated_chunks.chunks[index].start,allocated_chunks.chunks[index].size);
        chunk_list_remove(&allocated_chunks,(size_t)index);
    }
}

void heap_collect(void *ptr){

    UNIMPLEMENTED;
}

int main(){

    void *p=heap_alloc(3);
    p="abwertfgjyhkjnkmljlhbvdfxvgjhbjvgvhnjk";

    printf("%p size of p=%ld\n",*(&p),sizeof(*p));

    chunk_list_dump(&allocated_chunks);
    chunk_list_dump(&free_chunks);

    return 0;
}