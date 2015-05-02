/* treemap.h */
#ifndef DSTRUCTS_TREEMAP_H
#define DSTRUCTS_TREEMAP_H
#include "dstructs.h"

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
    int count;
    struct tree_node* root;
    key_comparator compar;
    destructor dstor;
};
struct treemap* treemap_new(key_comparator compar,destructor dstor);
struct treemap* treemap_new_ex(key_comparator compar,destructor dstor,void** keys,int size);
void treemap_free(struct treemap* treemap);
void treemap_init(struct treemap* treemap,key_comparator compar,destructor dstor);
void treemap_init_ex(struct treemap* treemap,key_comparator compar,destructor dstor,void** keys,int size);
void treemap_delete(struct treemap* treemap);
int treemap_insert(struct treemap* treemap,void* key);
void* treemap_lookup(struct treemap* treemap,const void* key);
int treemap_remove(struct treemap* treemap,const void* key);
int treemap_filter_count(struct treemap* treemap,key_filter_callback callback);
void treemap_traversal_inorder(struct treemap* treemap,key_callback callback);

#endif
