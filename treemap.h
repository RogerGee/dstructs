/* treemap.h */
#ifndef DSTRUCTS_TREEMAP_H
#define DSTRUCTS_TREEMAP_H

/* typedefs */
typedef int (*key_comparator)(const void* left,const void* right); /* compare keys 'left' and 'right' */
typedef void (*key_callback)(void* key); /* perform operation on 'key' when called */

/* provide basic key types that a user may use to implement a map */
struct tree_string_key
{
    char* key; /* string key value */
    void* payload; /* pointer to object; this object is deallocated by the key object */
};
void tree_string_key_init(struct tree_string_key* key,const char* keyval,void* payload);
void tree_string_key_delete(struct tree_string_key* key);
int tree_string_key_compare(const struct tree_string_key* left,const struct tree_string_key* right);
int tree_string_key_compare_insensitive(const struct tree_string_key* left,const struct tree_string_key* right);

struct tree_node;

/* represents a balanced tree structure for storing key objects by reference; a
   comparator must be used to compare two key object references; a destructor can
   be provided (set to NULL if not used) to delete the object before it is freed */
struct treemap
{
    struct tree_node* root;
    key_comparator compar;
    key_callback dstor;
};
struct treemap* treemap_new(key_comparator compar,key_callback dstor);
struct treemap* treemap_new_ex(key_comparator compar,key_callback dstor,void** keys,int size);
void treemap_free(struct treemap* treemap);
void treemap_init(struct treemap* treemap,key_comparator compar,key_callback dstor);
/* 'keys[n]' for every n must be heap-allocated */
void treemap_init_ex(struct treemap* treemap,key_comparator compar,key_callback dstor,void** keys,int size);
void treemap_delete(struct treemap* treemap);
int treemap_insert(struct treemap* treemap,void* key); /* 'key' must be heap-allocated */
void* treemap_lookup(struct treemap* treemap,const void* key);
int treemap_remove(struct treemap* treemap,const void* key);
void treemap_traversal_inorder(struct treemap* treemap,key_callback callback);

#endif
