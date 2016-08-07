/* hashmap.h */
#ifndef DSTRUCTS_HASHMAP_H
#define DSTRUCTS_HASHMAP_H
#include "dstructs.h"

/* typedefs for function types used by the hashmap */
typedef int (*hash_function)(const void* key,int size);

/* hash functions for common key types */
int hash_int(const int* key,int size);
int hash_string(const char* key,int size);
int hash_pstring(const char** key,int size);

struct hash_bucket;

/* represents a fixed-sized hash table that implements a map data structure; a
   user supplied hash-function generates an address for the key domain */
struct hashmap
{
    int hm_size;
    struct hash_bucket* hm_data;
    hash_function hm_hash;
    key_comparator hm_compar;
};
struct hashmap* hashmap_new(int size,hash_function hash,key_comparator compar);
void hashmap_free(struct hashmap* hm);
void hashmap_free_ex(struct hashmap* hm,destructor dstor);
void hashmap_init(struct hashmap* hm,int size,hash_function hash,key_comparator compar);
void hashmap_delete(struct hashmap* hm);
void hashmap_delete_ex(struct hashmap* hm,destructor dstor);
void hashmap_reset(struct hashmap* hm);
void hashmap_reset_ex(struct hashmap* hm,destructor dstor);
int hashmap_insert(struct hashmap* hm,void* key);
void* hashmap_lookup(struct hashmap* hm,const void* key);
int hashmap_remove(struct hashmap* hm,const void* key);
int hashmap_remove_ex(struct hashmap* hm,const void* key,destructor dstor);

#endif
