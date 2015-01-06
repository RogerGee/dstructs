/* dstructs.h */
#ifndef DSTRUCTS_H
#define DSTRUCTS_H

typedef void (*destructor)(void* elem);
typedef int (*key_comparator)(const void* left,const void* right); /* compare keys 'left' and 'right' */

#endif
