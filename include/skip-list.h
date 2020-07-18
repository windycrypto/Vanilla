#ifndef SKIP_LIST_H_
#define SKIP_LIST_H_

#define SKIP_LIST_MAX_LEVEL 32

struct skip_list_node {
    int key;
    struct skip_list_node *next[SKIP_LIST_MAX_LEVEL];
};

struct skip_list {
    struct skip_list_node head;
    struct skip_list_node tail;
};

#endif // SKIP_LIST_H_
