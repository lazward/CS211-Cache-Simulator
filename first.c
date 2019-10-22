#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "first.h"

int main(int argc, char** argv)
{

    int size = atoi(argv[1]) ; // A) <cache size>

    if (isPowerOfTwo(size) == 0) {

        printf("error") ;
        return 0 ;

    }

    int policy ; // C) <cache policy>

    if (strcmp(argv[3], "fifo") == 0) {

        policy = 0 ;

    } else {

        policy = 1 ;

    }

    int block = atoi(argv[4]) ; // D) <block size>

    if (isPowerOfTwo(block) == 0) {

        printf("error") ;
        return 0 ;

    }


    int assoc ;
    int lines = size / block ;
    int indexBits ;

    int sets ;

    // B) <assoociativity>

    if (strcmp(argv[2], "direct") == 0) { // Direct

        assoc = 1 ;

        sets = (size / block) ;


    } else if (strcmp(argv[2], "assoc") == 0) { // Fully

        assoc = 0 ;
        sets = 1 ;


    } else if (strlen(argv[2]) > 6) {

        char * a = (char *)malloc(strlen(argv[2]) * sizeof(char)) ;
        a = argv[2] ;
        char * s = (char *)malloc((strlen(a) - 5) * sizeof(char)) ;

        //memset(s, '\0', sizeof(s)) ;

        strncpy(s, a + 6, strlen(a) - 6) ;

        assoc = atoi(s) ;

        if (isPowerOfTwo(assoc) == 0) {

            printf("error") ;
            return 0 ;

        }

        //assoc ;
        sets = size / (assoc * block) ;

        //cache = (struct Node *)malloc(sets * sizeof(struct Node )) ;
        //cache->valid = 0 ;
        //cache->tag = 0 ;
        //cache->next = (struct Node*)malloc(sizeof(struct Node )) ;

    } else {

        printf("error") ;
        return 0 ;

    }

    indexBits = getLogBaseTwo(sets) ;

    struct hashTable * cache ;

    if (assoc > 0) {

    cache = createTable(sets, assoc) ;

    } else {

    cache = createTable(lines, assoc) ;

    }



    if (fopen(argv[5], "r") == NULL) {

        printf("error") ;
        return 0 ;

    }

    FILE *file = fopen(argv[5], "r") ;

    fseek(file, 0, SEEK_END) ;

    if (ftell(file) == 0) {

        printf("error") ;
        return 0 ;

    }

    fseek(file, 0, SEEK_SET) ;

    unsigned long long int a ; // doesn't matter

    char b ;

    unsigned long long int c ;

    int readCount = 0 ; // Memoory reads:
    int writeCount = 0 ; // Memory writes:
    int hitCount = 0 ; // Cache hits:
    int missCount = 0 ; // Cache misses:
    int readFetch = 0 ;
    int writeFetch = 0 ;
    int hitFetch = 0 ;
    int missFetch = 0 ;
    int usedCounter = 0 ;
    int addedCounter = 0 ;

    // Example: 0x804ae19: R 0x9cb3d40
    while (fscanf(file, "%llx: %c %llx", &a, &b, &c) == 3) {

        int tag = getTag(c, indexBits, block) ;

        if (b == 'W') {

            writeCount++ ;

        }

            if (assoc == 1 ) { // Direct

                int ind = getIndex(c, indexBits, block) ;

                struct Node * temp = cache->table[ind] ;

                if (temp->valid == 0) {
                    missCount++ ;
                    temp->valid = 1 ;
                    temp->tag = tag ;
                    readCount++ ;

                } else if (temp->tag == tag) {

                    hitCount++ ;

                } else {

                    missCount++ ;
                    temp->tag = tag ;
                    readCount++ ;

                }

            } else if (assoc == 0) {  // Fully

                struct Node * temp = cache->table[0] ;

                if (temp->valid == 0) {

                    missCount++ ;
                    temp->valid = 1 ;
                    temp->tag = tag ;
                    temp->used = usedCounter ;
                    temp->added = addedCounter ;
                    readCount++ ;

                } else if (temp->tag == tag) {

                    temp->used = usedCounter ;
                    hitCount++ ;

                } else {

                    struct Node *ptr = temp ;

                    while (ptr->next != NULL) {

                        if (ptr->tag == tag) {

                            break ;

                        }

                        ptr = ptr->next ;

                    }

                    if (ptr->tag == tag) {

                        ptr->used = usedCounter ;
                        hitCount++ ;

                    } else {

                        missCount++ ;
                        addToCache(temp, tag, policy, usedCounter, addedCounter, lines) ;
                        readCount++ ;

                    }



                }


        } else { // assoc:n

            int ind = getIndex(c, indexBits, block) ;

            struct Node * temp = cache->table[ind] ;

            if (temp->valid == 0) {

                    missCount++ ;
                    temp->valid = 1 ;
                    temp->tag = tag ;
                    temp->used = usedCounter ;
                    temp->added = addedCounter ;
                    readCount++ ;

            } else if (temp->tag == tag) {

                    temp->used = usedCounter ;
                    hitCount++ ;

                } else {

                    struct Node *ptr = temp ;

                    while (ptr->next != NULL) {

                        if (ptr->tag == tag) {

                            break ;

                        }

                        ptr = ptr->next ;

                    }

                    if (ptr->tag == tag) {

                        ptr->used = usedCounter ;
                        hitCount++ ;

                    } else {

                        missCount++ ;
                        addToCache(temp, tag, policy, usedCounter, addedCounter, lines) ;
                        readCount++ ;

                    }


        }

        }

        usedCounter++ ;
        addedCounter++ ;


        }

    //printf("prefetch\n") ;

    free(cache) ;
    usedCounter = 0 ;
    addedCounter = 0 ;

    if (assoc > 0) {

    cache = createTable(sets, assoc) ;

    } else {

    cache = createTable(lines, assoc) ;

    }

    fseek(file, 0, SEEK_SET) ;

    while (fscanf(file, "%llx: %c %llx", &a, &b, &c) == 3) {

        int tag = getTag(c, indexBits, block) ;

        if (b == 'W') {

            writeFetch++ ;

        }

            if (assoc == 1 ) { // Direct

                int ind = getIndex(c, indexBits, block) ;

                struct Node * temp = cache->table[ind] ;

                if (temp->valid == 0) {

                    missFetch++ ;
                    temp->valid = 1 ;
                    temp->tag = tag ;
                    readFetch++ ;

                    usedCounter++ ;
                    addedCounter++ ;

                    unsigned long long int fetch = c + block ;
                    int ftag = getTag(fetch, indexBits, block) ;
                    ind = getIndex(fetch, indexBits, block) ;
                    struct Node * f = cache->table[ind] ;

                    if (f->valid == 0) {

                        f->valid = 1;
                        f->tag = ftag ;
                        f->added = addedCounter ;
                        readFetch++ ;

                    } else if (f->tag != ftag) {

                        f->tag = ftag ;
                        readFetch++ ;

                    }

                } else if (temp->tag == tag) {

                    hitFetch++ ;

                } else {

                    missFetch++ ;
                    temp->tag = tag ;
                    readFetch++ ;

                    usedCounter++ ;
                    addedCounter++ ;

                    unsigned long long int fetch = c + block ;
                    int ftag = getTag(fetch, indexBits, block) ;
                    ind = getIndex(fetch, indexBits, block) ;
                    struct Node * f = cache->table[ind] ;

                    if (f->valid == 0) {

                        f->valid = 1;
                        f->tag = ftag ;
                        f->added = addedCounter ;
                        readFetch++ ;

                    } else if (f->tag != ftag) {

                        f->tag = ftag ;
                        f->added = addedCounter ;
                        readFetch++ ;

                    }

                }

            } else if (assoc == 0) {  // Fully

                struct Node * temp = cache->table[0] ;

                if (temp->valid == 0) {

                    missFetch++ ;
                    temp->valid = 1 ;
                    temp->tag = tag ;
                    temp->used = usedCounter ;
                    temp->added = addedCounter ;
                    readFetch++ ;

                    usedCounter++ ;
                    addedCounter++ ;

                    unsigned long long int fetch = c + block ;
                    int ftag = getTag(fetch, indexBits, block) ;
                    temp->next->tag = ftag ;
                    temp->next->valid = 1 ;
                    temp->next->used = usedCounter ;
                    temp->next->added = addedCounter ;
                    readFetch++ ;


                } else if (temp->tag == tag) {

                    temp->used = usedCounter ;
                    hitFetch++ ;

                } else {

                    struct Node *ptr = temp ;

                    while (ptr->next != NULL) {

                        if (ptr->tag == tag) {

                            break ;

                        }

                        ptr = ptr->next ;

                    }

                    if (ptr->tag == tag) {

                        ptr->used = usedCounter ;
                        hitFetch++ ;

                    } else {

                        missFetch++ ;
                        addToCache(temp, tag, policy, usedCounter, addedCounter, lines) ;
                        readFetch++ ;

                        usedCounter++ ;
                        addedCounter++ ;

                        ptr = temp ;

                        unsigned long long int fetch = c + block ;
                        int ftag = getTag(fetch, indexBits, block) ;

                        while (ptr->next != NULL) {

                            if (ptr->tag == ftag) {

                                break ;

                            }

                            ptr = ptr->next ;

                        }

                        if (ptr->tag != ftag) {

                            addToCache(temp, ftag, policy, usedCounter, addedCounter, lines) ;
                            readFetch++ ;

                        }

                    }



                }


        } else { // assoc:n

            int ind = getIndex(c, indexBits, block) ;

            struct Node * temp = cache->table[ind] ;

            if (temp->valid == 0) {

                    missFetch++ ;
                    temp->valid = 1 ;
                    temp->tag = tag ;
                    temp->used = usedCounter ;
                    temp->added = addedCounter ;
                    readFetch++ ;

                    usedCounter++ ;
                    addedCounter++ ;
                    unsigned long long int fetch = c + block ;
                    int ftag = getTag(fetch, indexBits, block) ;
                    ind = getIndex(fetch, indexBits, block) ;
                    struct Node * f = cache->table[ind] ;
                    struct Node *ptr = f ;

                        while (ptr->next != NULL) {

                            if (ptr->tag == ftag) {

                                break ;

                            }

                            ptr = ptr->next ;

                        }

                        if (ptr->tag != ftag) {

                            addToCache(f, ftag, policy, usedCounter, addedCounter, lines) ;
                            readFetch++ ;


                        }



            } else if (temp->tag == tag) {

                    temp->used = usedCounter ;
                    hitFetch++ ;

                } else {

                    struct Node *ptr = temp ;

                    while (ptr->next != NULL) {

                        if (ptr->tag == tag) {

                            break ;

                        }

                        ptr = ptr->next ;

                    }

                    if (ptr->tag == tag) {

                        ptr->used = usedCounter ;
                        hitFetch++ ;

                    } else {

                        missFetch++ ;
                        addToCache(temp, tag, policy, usedCounter, addedCounter, lines) ;
                        readFetch++ ;

                        usedCounter++ ;
                        addedCounter++ ;

                        unsigned long long int fetch = c + block ;
                        int ftag = getTag(fetch, indexBits, block) ;
                        ind = getIndex(fetch, indexBits, block) ;
                        struct Node * f = cache->table[ind] ;

                        ptr = f ;

                        while (ptr->next != NULL) {

                            if (ptr->tag == ftag) {

                                break ;

                            }

                            ptr = ptr->next ;

                        }

                        if (ptr->tag != ftag) {

                            addToCache(f, ftag, policy, usedCounter, addedCounter, lines) ;
                            readFetch++ ;


                        }



                    }


        }

        }

        usedCounter++ ;
        addedCounter++ ;

    }

    printf("no-prefetch\nMemory reads: %d\nMemory writes: %d\nCache hits: %d\nCache misses: %d\n", readCount, writeCount, hitCount, missCount) ;
    printf("with-prefetch\nMemory reads: %d\nMemory writes: %d\nCache hits: %d\nCache misses: %d\n", readFetch, writeFetch, hitFetch, missFetch) ;

    return 0;
}

