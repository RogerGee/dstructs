/* hashmap.c */
#include "hashmap.h"
#include <stdlib.h>

int hash_int(const int* key,int size)
{
    int i = *key % size;
    return i<0 ? -i : i;
}

int hash_string(const char* key,int size)
{
    /* string hash using djb2 algorithm */
    unsigned long hash = 5381;
    while (*key) {
        hash = ((hash<<5) + hash) + *key;
        ++key;
    }
    return hash%size;
}

int hash_pstring(const char** key,int size)
{
    unsigned long hash = 5381;
    const char* rkey = *key;
    while (*rkey) {
        hash = ((hash<<5) + hash) + *rkey;
        ++rkey;
    }
    return hash%size;    
}

struct hash_bucket
{
    void* key;
    struct hash_bucket* nxt;
};
static void hash_bucket_default(struct hash_bucket* hb)
{
    hb->key = NULL;
    hb->nxt = NULL;
}

struct hashmap* hashmap_new(int size,hash_function hash,key_comparator compar)
{
    struct hashmap* hm;
    hm = malloc(sizeof(struct hashmap));
    if (hm == NULL)
        return NULL;
    hashmap_init(hm,size,hash,compar);
    return hm;
}
void hashmap_free(struct hashmap* hm)
{
    hashmap_delete(hm);
    free(hm);
}
void hashmap_free_ex(struct hashmap* hm,destructor dstor)
{
    hashmap_delete_ex(hm,dstor);
    free(hm);
}
void hashmap_init(struct hashmap* hm,int size,hash_function hash,key_comparator compar)
{
    int i;
    hm->hm_data = malloc(sizeof(struct hash_bucket) * size);
    hm->hm_size = size;
    hm->hm_hash = hash;
    hm->hm_compar = compar;
    for (i = 0;i < size;++i)
        hash_bucket_default(hm->hm_data + i);
}
void hashmap_delete(struct hashmap* hm)
{
    int i;
    for (i = 0;i < hm->hm_size;++i) {
        struct hash_bucket* hb;
        hb = hm->hm_data[i].nxt;
        while (hb != NULL) {
            struct hash_bucket* tmp = hb->nxt;
            free(hb);
            hb = tmp;
        }
    }
    hm->hm_size = 0;
    free(hm->hm_data);
    hm->hm_data = NULL;
    hm->hm_hash = NULL;
    hm->hm_compar = NULL;
}
void hashmap_delete_ex(struct hashmap* hm,destructor dstor)
{
    int i;
    for (i = 0;i < hm->hm_size;++i) {
        if (hm->hm_data[i].key != NULL) {
            struct hash_bucket* hb;
            (*dstor)(hm->hm_data[i].key);
            hb = hm->hm_data[i].nxt;
            while (hb != NULL) {
                struct hash_bucket* tmp = hb->nxt;
                (*dstor)(hb->key);
                free(hb);
                hb = tmp;
            }
        }
    }
    hm->hm_size = 0;
    free(hm->hm_data);
    hm->hm_data = NULL;
    hm->hm_hash = NULL;
    hm->hm_compar = NULL;
}
void hashmap_reset(struct hashmap* hm)
{
    int i;
    for (i = 0;i < hm->hm_size;++i) {
        if (hm->hm_data[i].key != NULL) {
            struct hash_bucket* hb;
            hb = hm->hm_data[i].nxt;
            while (hb != NULL) {
                struct hash_bucket* tmp = hb->nxt;
                free(hb);
                hb = tmp;
            }
            hash_bucket_default(hm->hm_data + i);
        }
    }
}
void hashmap_reset_ex(struct hashmap* hm,destructor dstor)
{
    int i;
    for (i = 0;i < hm->hm_size;++i) {
        if (hm->hm_data[i].key != NULL) {
            struct hash_bucket* hb;
            (*dstor)(hm->hm_data[i].key);
            hb = hm->hm_data[i].nxt;
            while (hb != NULL) {
                struct hash_bucket* tmp = hb->nxt;
                (*dstor)(hb->key);
                free(hb);
                hb = tmp;
            }
            hash_bucket_default(hm->hm_data + i);
        }
    }
}
int hashmap_insert(struct hashmap* hm,void* key)
{
    int index;
    index = (*hm->hm_hash)(key,hm->hm_size);
    if (hm->hm_data[index].key == NULL)
        hm->hm_data[index].key = key;
    else {
        struct hash_bucket* last;
        /* collision; insert the key at the end of the linked list of buckets */
        last = hm->hm_data + index;
        while (1) {
            /* make sure that the key doesn't already exist in the map */
            if ((*hm->hm_compar)(last->key,key) == 0)
                return 1;
            if (last->nxt == NULL)
                break;
            last = last->nxt;
        }
        last->nxt = malloc(sizeof(struct hash_bucket));
        last->nxt->key = key;
        last->nxt->nxt = NULL;
    }
    return 0;
}
void* hashmap_lookup(struct hashmap* hm,const void* key)
{
    int index;
    index = (*hm->hm_hash)(key,hm->hm_size);
    if (hm->hm_data[index].key != NULL) {
        struct hash_bucket* bucket;
        bucket = &hm->hm_data[index];
        while (1) {
            if ((*hm->hm_compar)(bucket->key,key) == 0)
                return bucket->key;
            if (bucket->nxt == NULL)
                break;
            bucket = bucket->nxt;
        }
    }
    return NULL;
}
static void* hashmap_remove_generic(struct hashmap* hm,const void* key)
{
    int index;
    index = (*hm->hm_hash)(key,hm->hm_size);
    if (hm->hm_data[index].key != NULL) {
        void* r;
        struct hash_bucket* del, *prev;
        if ((*hm->hm_compar)(hm->hm_data[index].key,key) == 0) {
            r = hm->hm_data[index].key;
            if (hm->hm_data[index].nxt != NULL) {
                del = hm->hm_data[index].nxt;
                hm->hm_data[index].key = del->key;
                hm->hm_data[index].nxt = del->nxt;
                free(del);
            }
            else
                hm->hm_data[index].key = NULL;
            return r;
        }
        prev = hm->hm_data + index;
        del = prev->nxt;
        while (del != NULL) {
            if ((*hm->hm_compar)(del->key,key) == 0) {
                r = del->key;
                prev->nxt = del->nxt;
                free(del);
                return r;
            }
            prev = del;
            del = del->nxt;
        }
    }
    return NULL;
}
int hashmap_remove(struct hashmap* hm,const void* key)
{
    void* result;
    result = hashmap_remove_generic(hm,key);
    if (result == NULL)
        return 1;
    return 0;
}
int hashmap_remove_ex(struct hashmap* hm,const void* key,destructor dstor)
{
    void* result;
    result = hashmap_remove_generic(hm,key);
    if (result == NULL)
        return 1;
    (*dstor)(result);
    return 0;    
}
