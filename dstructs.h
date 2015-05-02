/* dstructs.h */
#ifndef DSTRUCTS_H
#define DSTRUCTS_H

typedef void (*destructor)(void* elem); /* perform cleanup on element */
typedef int (*key_comparator)(const void* left,const void* right); /* compare keys 'left' and 'right' */
typedef void (*key_callback)(void* key); /* perform operation on 'key' when called */
typedef int (*key_filter_callback)(void* key); /* perform operation on 'key' when called (used for filtering keys) */

/* common comparators */
static inline int intcmp(const int* left,const int* right)
{ return *left - *right; }

#endif
