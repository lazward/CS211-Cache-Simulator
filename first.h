int isPowerOfTwo(int i) {

    if (i == 0) {

    return 0 ;

    }

    int j = i ;

    while (j != 1) {

        if (j % 2 != 0) {

            return 0 ;

        }

        j = j / 2 ;

    }

    return 1 ;

}

struct Node {

    int valid ;
    int tag ;
    struct Node *next ;
    int used ;
    int added ;


};

struct hashTable {

    struct Node ** table ;

};

struct hashTable * createTable(int s, int a) {

    struct hashTable * output = (struct hashTable *)malloc(sizeof(struct hashTable)) ;

    if (a > 0) {

        output->table = (struct Node **)malloc(s * sizeof(struct Node * ) ) ;

        int i, j ;
        for (i = 0 ; i < s ; i++) {

            output->table[i] = (struct Node *)malloc(sizeof(struct Node )) ;

            output->table[i]->valid = 0 ;

            struct Node * ptr = output->table[i] ;

            for (j = 1 ; j < a ; j++) {

                ptr->next = (struct Node *)malloc(sizeof(struct Node )) ;
                ptr->next->valid = 0 ;
                ptr = ptr->next ;

            }

            }

    } else {

        output->table = (struct Node **)malloc(sizeof(struct Node * ) ) ;
        //output->table[0] = (struct Node *)malloc(s * sizeof(struct Node )) ;
        //output->table[0]-> valid = 0 ;

        //struct Node * ptr = output->table[0] ;

        output->table[0] = (struct Node *)malloc(sizeof(struct Node)) ;
        output->table[0]->valid = 0 ;

        int i ;
        struct Node * ptr = output->table[0] ;

            for (i = 1 ; i < s ; i++) {

                ptr->next = (struct Node *)malloc(sizeof(struct Node )) ;
                ptr->next->valid = 0 ;
                ptr = ptr->next ;

            }

    }

    return output ;


};

int getLogBaseTwo(int x) {

    int i = x ;
    int output = 0 ;

    while (i >>= 1) {

    output++ ;

    }

    return output ;

}

void addToCache(struct Node * c, int t, int pol, int u, int a, int s) { // cache, tag, policy, used/added counters and size

    struct Node * temp = c;

    struct Node * ptr = temp ;

    while (ptr->next != NULL && ptr->valid == 1) {

        ptr = ptr->next ;

    }

    if (ptr->valid == 0) {

        ptr->valid = 1 ;
        ptr->tag = t ;
        ptr->used = u ;
        ptr->added = a ;

    } else {

        if (pol == 0) { // FIFO

            ptr = temp ;
            int first = temp->added ;

            while (ptr != NULL && ptr->valid == 1) {

                if (ptr->added < first) {

                   first = ptr->added ;

                }

                ptr = ptr->next ;

            }

            ptr = temp ;

            while (ptr->added != first) {

                ptr = ptr->next ;

            }

            ptr->tag = t ;
            ptr->used = u ;
            ptr->added = a ;


        } else { // LRU

            ptr = temp ;
            int lru = temp->used ;

            while (ptr != NULL && ptr->valid == 1) {

                if (ptr->used < lru) {

                   lru = ptr->used ;

                }

                ptr = ptr->next ;

            }

            ptr = temp ;

            while (ptr->used != lru) {

                ptr = ptr->next ;

            }

            ptr->tag = t ;
            ptr->used = u ;
            ptr->added = a ;


        }

    }


}

int getIndex(unsigned long long int a, int s, int b) { // input address, assoc and block

        int index = s ;
        int block = getLogBaseTwo(b) ;

        int i = (a >> block) & ((1 << index) - 1) ;

        return i ;

}

int getTag(unsigned long long int a, int s, int b) {

        int index = s ;
        int block = getLogBaseTwo(b) ;

        int tag = a >> (index + block) ;

        return tag ;


}

