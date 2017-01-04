/* dstructs.h */
#ifndef DSTRUCTS_H
#define DSTRUCTS_H
#include <stdbool.h>

/* Performs cleanup on element */
typedef void (*destructor)(void* elem);

/* Compares keys 'left' and 'right'. */
typedef int (*key_comparator)(const void* left,const void* right);

/* Performs operation on 'key' when called. Extended version allows for callback
 * info to be passed.
 */
typedef void (*key_callback)(void* key);
typedef void (*key_callback_ex)(void* key,void* data);

/* Perform operation on 'key' when called (used for filtering keys). */
typedef int (*key_filter_callback)(void* key);

/* common comparators */
static inline int intcmp(const int* left,const int* right)
{ return *left - *right; }

#endif
