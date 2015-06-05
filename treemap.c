/* treemap.c - implements 2-3 tree-based map data structure */
#include "treemap.h"
#include "dynarray.h"
#include <stdlib.h>
#include <string.h>

/* structures used by the implementation */
struct tree_node
{
    void* keys[3];
    struct tree_node* children[4];
};
struct key_info
{
    key_comparator compar;
    destructor dstor;
};
struct key_impl_info
{
    void* key;
    struct key_info* keyinfo;
};
struct search_impl_info
{
    int index;
    struct tree_node* node;
    struct key_impl_info info;
};

/* tree_string_key */
void tree_string_key_init(struct tree_string_key* key,const char* keyval,void* payload)
{
    int i;
    int len;
    len = strlen(keyval)+1;
    key->key = malloc(len);
    for (i = 0;i < len;++i)
        key->key[i] = keyval[i];
    key->payload = payload;
}
void tree_string_key_delete(struct tree_string_key* key)
{
    free(key->key);
    if (key->payload != NULL)
        free(key->payload);
}
int tree_string_key_compare(const struct tree_string_key* left,const struct tree_string_key* right)
{
    const char* l = left->key;
    const char* r = right->key;
    while (*l) {
        if (*l != *r)
            return *l - *r;
        ++l, ++r;
    }
    return *l - *r;
}
int tree_string_key_compare_insensitive(const struct tree_string_key* left,const struct tree_string_key* right)
{
    const char* l = left->key;
    const char* r = right->key;
    while (*l) {
        short i;
        char c[2];
        c[0] = *l;
        c[1] = *r;
        for (i = 0;i < 2;++i)
            if (c[i]>='A' && c[i]<='Z')
                c[i] = c[i]-'A' + 'a';
        if (c[0] != c[1])
            return c[0] - c[1];
        ++l, ++r;
    }
    return *l - *r;
}

/* tree_node */
static void tree_node_init(struct tree_node* node)
{
    int i;
    for (i = 0;i < 3;++i)
        node->keys[i] = NULL;
    for (i = 0;i < 4;++i)
        node->children[i] = NULL;
}
static void tree_node_delete(struct tree_node* node)
{
    /* this function does not delete the node's keys; that task is left
       up to the user; the references are simply discarded as the tree_node
       object is about to be deleted */
    int i;
    for (i = 0;i < 4;++i) {
        if (node->children[i] != NULL) {
            tree_node_delete(node->children[i]);
            free(node->children[i]);
        }
    }
}
static void tree_node_delete_dstor(struct tree_node* node,destructor dstor)
{
    /* this function calls a user-provided destructor on each key object */
    int i;
    for (i = 0;i < 3;++i)
        if (node->keys[i] != NULL)
            (*dstor)(node->keys[i]);
    for (i = 0;i < 4;++i) {
        if (node->children[i] != NULL) {
            tree_node_delete_dstor(node->children[i],dstor);
            free(node->children[i]);
        }
    }
}
static int tree_node_insert_key(struct tree_node* node,struct key_impl_info* info,struct tree_node* left,struct tree_node* right)
{
    int i, j;
    int cmp;
    if (node->keys[0] == NULL)
        cmp = -1;
    else
        cmp = (*info->keyinfo->compar)(info->key,node->keys[0]);
    if (cmp == 0)
        return 1;
    if (cmp < 0) {
        /* key is inserted into position 0; shift elements over */
        for (i = 1,j = 2;i >= 0;--i,--j)
            node->keys[j] = node->keys[i];
        for (i = 2, j = 3;i >= 0;--i,--j)
            node->children[j] = node->children[i];
        node->keys[0] = info->key;
        node->children[0] = left;
        node->children[1] = right;
    }
    else if (node->keys[1] != NULL) {
        cmp = (*info->keyinfo->compar)(info->key,node->keys[1]);
        if (cmp == 0)
            return 1;
        if (cmp < 0) {
            /* key is inserted into position 1; shift elements over */
            node->keys[2] = node->keys[1];
            node->children[3] = node->children[2];
            node->keys[1] = info->key;
            node->children[1] = left;
            node->children[2] = right;
        }
        else {
            /* key is inserted into position 2; no shift required */
            node->keys[2] = info->key;
            node->children[2] = left;
            node->children[3] = right;
        }
    }
    else {
        /* key is inserted into position 1; no shift required */
        node->keys[1] = info->key;
        node->children[1] = left;
        node->children[2] = right;
    }
    return 0;
}
static void tree_node_do_split(struct tree_node* node,struct tree_node* parent,struct key_info* info)
{
    /* this procedure assumes that the caller has ensured that 'node' is a 4-node */
    int i, j;
    struct tree_node* newnode;
    struct key_impl_info insinfo; /* insinfo.key is median key value */
    /* the node is already sorted, so choose keys[1] as the median */
    insinfo.key = node->keys[1];
    /* 'newnode' is going to be the right value; assign children from old node to it; overwrite
       children and keys in 'node' with NULLs so that it becomes a 2-node, thus making 'node'
       the left value */
    newnode = malloc(sizeof(struct tree_node));
    tree_node_init(newnode);
    newnode->keys[0] = node->keys[2];
    for (i = 0,j = 2;i <= 1;++i,++j) {
        newnode->children[i] = node->children[j];
        node->children[j] = NULL;
    }
    for (i = 1;i <= 2;++i)
        node->keys[i] = NULL;
    /* insert the node up into the parent; insertion should succeed because split value should not exist in parent */
    insinfo.keyinfo = info;
    tree_node_insert_key(parent,&insinfo,node,newnode);
}
static void* tree_node_delete_key(struct tree_node* node,struct key_impl_info* info,int index)
{
    /* assume that 'node->keys[index]' exists; this procedure leaves a hole
       in a leave node in some subtree of 'node' to be fixed later; return 
       the search key for the caller if a hole was placed */
    void* swapKey;
    struct tree_node* n;
    void* k;
    swapKey = NULL;
    /* make 'n' point to right subtree; use k to save key to delete */
    n = node->children[index+1];
    k = node->keys[index];
    /* handle case where 'node' is a leaf */
    if (n == NULL) {
        if (node->keys[1] != NULL) {
            /* remove key from 3-node */
            node->keys[index] = node->keys[index+1];
            node->keys[index+1] = NULL;
        }
        else {
            /* leave hole */
            node->keys[index] = NULL;
            swapKey = info->key;
        }
    }
    /* handle case where 'node' is internal; swap the key to delete with the next-greater key */
    else {
        /* find next-greatest key in tree by searching for smallest element
           of the right subtree; 'n' refers to the root of this subtree */
        while (n->children[0] != NULL)
            n = n->children[0];
        /* if 'n' now points to a leaf node; take its least-ordered key and assign it to the node;
           if 'n' is a 3-node, shift the greater-ordered element over; else if 'n' is a 2-node,
           leave a hole to be fixed later */
        node->keys[index] = n->keys[0];
        if (n->keys[1] != NULL) {
            n->keys[0] = n->keys[1];
            n->keys[1] = NULL;
        }
        else {
            /* leave hole */
            n->keys[0] = NULL;
            swapKey = node->keys[index];
        }
    }
    /* delete the key 'k': call its destructor if it was specified */
    if (info->keyinfo->dstor != NULL)
        (*info->keyinfo->dstor)(k);
    return swapKey;
}
static void tree_node_remove_key(struct tree_node* node,int keyIndex,int childIndex)
{
    int i, j;
    for (i = keyIndex,j = keyIndex+1;j < 3;++i,++j)
        node->keys[i] = node->keys[j];
    for (i = childIndex,j = childIndex+1;j < 4;++i,++j)
        node->children[i] = node->children[j];
}
static void tree_node_do_fix(struct tree_node* node,struct tree_node* parent)
{
    /* this procedure assumes that the caller has ensured that 'node' is a hole */
    int i;
    int ni;
    int bound;
    ni = 0;
    while (parent->children[ni] != node)
        ++ni;
    if (parent->keys[1] != NULL)
        bound = 2;
    else
        bound = 1;
    /* see if immediate right sibling is a 3-node */
    if (ni<bound && parent->children[(i = ni+1)]->keys[1] != NULL) {
        /* separator in parent is at index 'ni' */
        node->keys[0] = parent->keys[ni];
        parent->keys[ni] = parent->children[i]->keys[0];
        node->children[1] = parent->children[i]->children[0];
        tree_node_remove_key(parent->children[i],0,0);
    }
    /* see if immediate left sibling is a 3-node */
    else if (ni>0 && parent->children[(i = ni-1)]->keys[1] != NULL) {
        /* separator in parent is at index i */
        node->keys[0] = parent->keys[i];
        parent->keys[i] = parent->children[i]->keys[1];
        node->children[1] = node->children[0]; /* need to shift this over */
        node->children[0] = parent->children[i]->children[2];
        tree_node_remove_key(parent->children[i],1,2);
    }
    /* any immediate siblings are 2-nodes */
    else {
        int left, right;
        if (ni > 0) {
            left = ni-1;
            right = ni;
        }
        else {
            left = 0;
            right = 1;
        }
        if (parent->children[left]->keys[0] != NULL) {
            /* left is 2-node; right is hole */
            parent->children[left]->keys[1] = parent->keys[left];
            parent->children[left]->children[2] = parent->children[right]->children[0];
        }
        else {
            /* left is hole; right is 2-node */
            parent->children[left]->keys[0] = parent->keys[left];
            parent->children[left]->keys[1] = parent->children[right]->keys[0];
            for (i = 0;i<2;++i)
                parent->children[left]->children[i+1] = parent->children[right]->children[i];
        }
        /* delete the right child node */
        free(parent->children[right]);
        parent->children[right] = NULL;
        /* remove the separator from the parent; shift children over from positions >right */
        tree_node_remove_key(parent,left,right);
        /* parent may now be a hole node */
    }
}

/* treemap */
struct treemap* treemap_new(key_comparator compar,destructor dstor)
{
    struct treemap* treemap;
    treemap = malloc(sizeof(struct treemap));
    if (treemap == NULL)
        return NULL;
    treemap_init(treemap,compar,dstor);
    return treemap;
}
struct treemap* treemap_new_ex(key_comparator compar,destructor dstor,void** keys,int size)
{
    struct treemap* treemap;
    treemap = malloc(sizeof(struct treemap));
    if (treemap == NULL)
        return NULL;
    treemap_init_ex(treemap,compar,dstor,keys,size);
    return treemap;
}
void treemap_free(struct treemap* treemap)
{
    if (treemap != NULL) {
        treemap_delete(treemap);
        free(treemap);
    }
}
void treemap_init(struct treemap* treemap,key_comparator compar,destructor dstor)
{
    treemap->count = 0;
    treemap->root = NULL;
    treemap->compar = compar;
    treemap->dstor = dstor;
}
static void treemap_init_ex_recursive(struct tree_node** nodes,int size,void*** level)
{
    int i, j;
    struct dynamic_array* arr;
    if (*(level-1) != NULL)
        /* at internal level: prepare queue for child nodes */
        arr = dynamic_array_new_ex(size*2);
    else
        /* at leaf level: nodes have no children */
        arr = NULL;
    for (i = 0,j = 0;i < size;++i) {
        int k;
        k = 0;
        /* place keys into the node; a NULL value acts as the terminator for any
           node's keys; allow the node to become up to a 4-node */
        while (k < 3) {
            if ((*level)[j] == NULL) {
                ++j;
                break;
            }
            nodes[i]->keys[k++] = (*level)[j++];
        }
        /* create the node's children according to tree behavior; allow up to 4 children
           for 4-nodes */
        if (arr != NULL) {
            k = 0;
            while (k<2 || (k<4 && nodes[i]->keys[k-1]!=NULL)) {
                struct tree_node* n;
                n = malloc(sizeof(struct tree_node));
                dynamic_array_pushback(arr,n);
                tree_node_init(n);
                nodes[i]->children[k++] = n;
            }
        }
    }
    if (arr != NULL) {
        /* recursively construct the next level (down the array) */
        treemap_init_ex_recursive((struct tree_node**)arr->da_data,arr->da_top,level-1);
        dynamic_array_free(arr);
    }
}
void treemap_init_ex(struct treemap* treemap,key_comparator compar,destructor dstor,void** keys,int size)
{
    /* 'keys' must be an array of pointers of size 'size' that point to
       individual tree key structures allocated on the heap */
    int i, j, k, sz;
    void** arr, *plast;
    struct dynamic_array* levels;
    /* check size requirements */
    treemap->count = size<0 ? 0 : size;
    if (size <= 0)
        return;
    /* we must ensure that the data is sorted */
    qsort(keys,size,sizeof(void*),compar);
    /* copy key structures to the array 'arr' that will be used in the tree; disallow duplicate key values,
       leaving them in array 'keys'; use NULL to terminate the arrays 'arr' and 'keys'; the user should check
       'keys' after this call to see if any keys were left unused */
    arr = malloc(sizeof(void*) * (size+1)); /* allocate 1 more for NULL terminator */
    i = 0;
    j = 0;
    k = 0;
    plast = NULL;
    while (1) {
        if (plast != NULL)
            while (j<size && (*compar)(plast,keys[j])==0)
                keys[k++] = keys[j++];
        if (j >= size)
            break;
        arr[i] = keys[j];
        plast = keys[j];
        ++i, ++j;
    }
    keys[k] = NULL;
    arr[i] = NULL;
    size = i;
    /* construct the levels of the tree; store them in a dynamic array; put NULL as first element;
       this will indicate the end for the implementation */
    levels = dynamic_array_new();
    dynamic_array_pushback(levels,NULL);
    dynamic_array_pushback(levels,arr);
    /* for each level: calculate the size of the new keys array; since keys are divided into groups of 3,
       we can allocate size/3 number of key-value structures */
    while ((sz = size / 3) > 0) {
        void** prev;
        /* cache the last level */
        prev = arr;
        /* add 2 to 'sz' so that we can: 1) store a terminating NULL pointer and 2) possibly
           store an extra key given that size%3 == 2 */
        sz += 2;
        arr = malloc(sizeof(void*) * sz);
        /* go through previous level; find the middle key of every 3-key pair; put it in 'arr';
           size of 'arr' will be 'i' */
        for (i = 0,j = 2;j < size;++i,j+=3) {
            arr[i] = prev[j-1];
            /* use NULL to separate keys */
            prev[j-1] = NULL;
        }
        /* handle case where size%3 == 2 */
        if (j == size) {
            k = size-2;
            arr[i++] = prev[k];
            prev[k] = NULL;
        }
        arr[i] = NULL;
        size = i;
        /* push back the array into 'levels' */
        dynamic_array_pushback(levels,arr);
    }
    /* construct the tree; first build the root node */
    treemap->root = malloc(sizeof(struct tree_node));
    treemap->compar = compar;
    treemap->dstor = dstor;
    tree_node_init(treemap->root);
    treemap_init_ex_recursive(&treemap->root,1,(void***)(levels->da_data + levels->da_top - 1));
    /* free allocated memory */
    dynamic_array_free_ex(levels,&free);
}
void treemap_delete(struct treemap* treemap)
{
    if (treemap->root != NULL) {
        if (treemap->dstor != NULL)
            tree_node_delete_dstor(treemap->root,treemap->dstor);
        else
            tree_node_delete(treemap->root);
        free(treemap->root);
    }
}
static int treemap_insert_recursive(struct tree_node** node,struct tree_node* parent,struct key_impl_info* info)
{
    struct tree_node* n;
    n = *node;
    if (n->children[0] != NULL) {
        /* 'node' is an internal node, which means it is at least
           a 2-node; recursively search for a leaf */
        int cmp;
        cmp = (*info->keyinfo->compar)(info->key,n->keys[0]);
        /* if 'key' already exists, return with error status */
        if (cmp == 0)
            return 1;
        if (cmp < 0) {
            /* 'key' is less than first key in node; visit left subtree */
            if (treemap_insert_recursive(n->children,n,info) == 1)
                return 1;
        }
        else if (n->keys[1] != NULL) {
            /* 'node' is a 3-node */
            cmp = (*info->keyinfo->compar)(info->key,n->keys[1]);
            if (cmp == 0)
                return 1;
            if (cmp < 0) {
                /* key is less than second key (greater than first); visit middle subtree */
                if (treemap_insert_recursive(n->children+1,n,info) == 1)
                    return 1;
            }
            else {
                /* key is greater than first and second keys; visit right subtree */
                if (treemap_insert_recursive(n->children+2,n,info) == 1)
                    return 1;
            }
        }
        else {
            /* key is greater than first key; visit right (middle) subtree */
            if (treemap_insert_recursive(n->children+1,n,info) == 1)
                return 1;
        }
    }
    /* 'node' is a leaf node; insert the key into it */
    else if (tree_node_insert_key(n,info,NULL,NULL) == 1)
        return 1;
    /* test if 'node' is now a 4-node, in which case it needs to be split */
    if (n->keys[2] != NULL) {
        /* if 'parent' is NULL, then 'node' is the root; we need to create a new root node */
        if (parent == NULL) {
            parent = malloc(sizeof(struct tree_node));
            tree_node_init(parent);
            /* assign the new root */
            *node = parent;
        }
        tree_node_do_split(n,parent,info->keyinfo);
    }
    /* return success status */
    return 0;
}
int treemap_insert(struct treemap* treemap,void* key)
{
    /* the user owns 'key' until we successfully add it to the tree;
       if the insert operation fails then the user is responsible for it */
    struct key_info kinfo;
    struct key_impl_info info;
    /* if the root is null, create the first node */
    if (treemap->root == NULL) {
        treemap->root = malloc(sizeof(struct tree_node));
        tree_node_init(treemap->root);
        treemap->root->keys[0] = key;
        ++treemap->count;
        return 0;
    }
    kinfo.compar = treemap->compar;
    kinfo.dstor = treemap->dstor;
    info.key = key;
    info.keyinfo = &kinfo;
    /* recursively insert element into tree */
    if (treemap_insert_recursive(&treemap->root,NULL,&info) == 1)
        return 1;
    ++treemap->count;
    return 0;
}
static void treemap_search_recursive(struct search_impl_info* info)
{
    /* since we use tail-recursion we can simply pass the same search_impl_info
       structure along to each recursive call (instead of allocating a new one
       on the stack each time) */
    int cmp;
    if (info->node == NULL)
        return;
    cmp = (*info->info.keyinfo->compar)(info->info.key,info->node->keys[0]);
    if (cmp == 0) {
        /* found key; mark which slot it's in */
        info->index = 0;
        return;
    }
    if (cmp < 0) {
        /* search left-subtree */
        info->node = info->node->children[0];
        treemap_search_recursive(info);
        return;
    }
    if (info->node->keys[1] != NULL) {
        /* 'info->node' is a 3-node */
        cmp = (*info->info.keyinfo->compar)(info->info.key,info->node->keys[1]);
        if (cmp == 0) {
            /* found key; mark which slot it's in */
            info->index = 1;
            return;
        }
        if (cmp < 0) {
            /* search middle-subtree */
            info->node = info->node->children[1];
            treemap_search_recursive(info);
            return;
        }
        /* search right-subtree */
        info->node = info->node->children[2];
        treemap_search_recursive(info);
        return;
    }
    /* search right-subtree of 2-node */
    info->node = info->node->children[1];
    treemap_search_recursive(info);
}
void* treemap_lookup(struct treemap* treemap,const void* key)
{
    /* 'key' is a key structure allocated to store a copy of the search key;
       we compare their values not addresses; we return the address of the key
       that is currently in the tree */
    struct key_info kinfo;
    struct search_impl_info info;
    kinfo.compar = treemap->compar;
    kinfo.dstor = treemap->dstor;
    info.node = treemap->root;
    info.info.key = (void*)key;
    info.info.keyinfo = &kinfo;
    treemap_search_recursive(&info);
    if (info.node != NULL)
        return info.node->keys[info.index];
    return NULL;
}
static void treemap_repair_recursive(struct tree_node** node,struct tree_node* parent,struct key_impl_info* info)
{
    /* recursive case: search for the hole */
    if ((*node)->keys[0] != NULL ) {
        if (info == NULL)
            /* search for the hole at the bottom of the left subtree */
            treemap_repair_recursive((*node)->children,(*node),NULL);
        else {
            /* search for the key */
            int cmp;
            cmp = (*info->keyinfo->compar)(info->key,(*node)->keys[0]);
            if (cmp == 0)
                /* found: search right subtree for successor slot */
                treemap_repair_recursive((*node)->children+1,(*node),NULL);
            else if (cmp < 0)
                treemap_repair_recursive((*node)->children,(*node),info);
            else if ((*node)->keys[1] != NULL) {
                /* '*node' is a 3-node */
                cmp = (*info->keyinfo->compar)(info->key,(*node)->keys[1]);
                if (cmp == 0)
                    /* found: search right subtree for successor slot */
                    treemap_repair_recursive((*node)->children+2,(*node),NULL);
                else if (cmp < 0)
                    treemap_repair_recursive((*node)->children+1,(*node),info);
                else
                    treemap_repair_recursive((*node)->children+2,(*node),info);
            }
            else
                treemap_repair_recursive((*node)->children+1,(*node),info);
        }
        if ((*node)->keys[0] != NULL)
            return;
    }
    if (parent == NULL) {
        /* root node is a hole-node; delete the root node and assign its sole
           child as the new root */
        parent = (*node);
        *node = parent->children[0];
        free(parent);
    }
    else {
        /* base case: fix a hole */
        tree_node_do_fix((*node),parent);
    }
}
int treemap_remove(struct treemap* treemap,const void* key)
{
    struct key_info kinfo;
    struct search_impl_info info;
    kinfo.compar = treemap->compar;
    kinfo.dstor = treemap->dstor;
    info.node = treemap->root;
    info.info.key = (void*)key;
    info.info.keyinfo = &kinfo;
    treemap_search_recursive(&info);
    if (info.node != NULL) {
        if ((info.info.key = tree_node_delete_key(info.node,&info.info,info.index)) != NULL)
            /* repair tree if removal left hole in tree */
            treemap_repair_recursive(&treemap->root,NULL,&info.info);
        --treemap->count;
        return 0;
    }
    return 1;
}
static void treemap_traversal_inorder_recursive(struct tree_node* node,key_callback callback)
{
    int i;
    i = 0;
    while (node->keys[i] != NULL) {
        if (node->children[i] != NULL)
            treemap_traversal_inorder_recursive(node->children[i],callback);
        (*callback)(node->keys[i]);
        ++i;
    }
    if (node->children[i] != NULL)
        treemap_traversal_inorder_recursive(node->children[i],callback);
}
void treemap_traversal_inorder(struct treemap* treemap,key_callback callback)
{
    if (treemap->root != NULL)
        treemap_traversal_inorder_recursive(treemap->root,callback);
}
static void treemap_filter_count_recursive(struct tree_node* node,int* num,key_filter_callback callback)
{
    int i;
    i = 0;
    while (node->keys[i] != NULL) {
        if (node->children[i] != NULL)
            treemap_filter_count_recursive(node->children[i],num,callback);
        if ( (*callback)(node->keys[i]) )
            ++ (*num);
        ++i;
    }
    if (node->children[i] != NULL)
        treemap_filter_count_recursive(node->children[i],num,callback);
}
int treemap_filter_count(struct treemap* treemap,key_filter_callback callback)
{
    int num = 0;
    if (treemap->root != NULL)
        treemap_filter_count_recursive(treemap->root,&num,callback);
    return num;
}
