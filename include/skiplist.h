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

struct skip_list *skip_list_new();
void skip_list_insert(struct skip_list *list, int key);
void skip_list_remove(struct skip_list *list, int key);
struct skip_list_node *skip_list_find(struct skip_list *list, int key);

#endif  // SKIP_LIST_H_
