#include "include/skip-list.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

/* An implementation of the
 * "Skip Lists: A Probabilistic Alternative to Balanced Trees" written by
 * William Pugh */

static inline struct skip_list_node *skip_list_new_node(int key) {
  struct skip_list_node *new;
  new = calloc(1, sizeof *new);
  new->key = key;
  return new;
}

struct skip_list *skip_list_new(/* parameter: max value of the key type */) {
  struct skip_list *list;

  list = calloc(1, sizeof *list);
  if (!list) abort();

  list->head.key = INT_MIN;
  list->tail.key = INT_MAX;

  for (int i = 0; i < SKIP_LIST_MAX_LEVEL; i++)
    list->head.next[i] = &list->tail;

  return list;
}

void insert(struct skip_list *list, int key) {
  struct skip_list_node *n = &list->head,  // a cursor used to traverse the list
      *prev[SKIP_LIST_MAX_LEVEL] = {NULL};  // stores n's previous nodes
  int lvl;

  // list should not be NULL if the user create the skip list using
  // `skip_list_new`
  if (!list) abort();

  lvl = 0;
  for (; lvl < SKIP_LIST_MAX_LEVEL; lvl++) prev[lvl] = &list->head;

  lvl = SKIP_LIST_MAX_LEVEL - 1;
  while (lvl >= 0) {
    // TODO allow duplicated node
    if (n->key == key) return;

    if (n->key < key) {
      prev[lvl] = n;
      n = n->next[lvl];
    } else
      n = prev[lvl--];
  }

  // till now, prev[0] points to the node just smaller than key
  struct skip_list_node *new = skip_list_new_node(key);
  lvl = 0;
  do {
    new->next[lvl] = prev[lvl]->next[lvl];
    prev[lvl]->next[lvl] = new;
    lvl++;

    // flip coin to decide how tall the levels the new node should be
    if (random() % 2) return;
  } while (lvl < SKIP_LIST_MAX_LEVEL);
}

void delete (struct skip_list *list, int key) {
  struct skip_list_node *n = &list->head,  // a cursor used to traverse the list
      *prev[SKIP_LIST_MAX_LEVEL] = {&list->head},  // stores n's previous nodes
          *target = NULL;

  int lvl = SKIP_LIST_MAX_LEVEL - 1;

  // list should not be NULL if the user create the skip list using
  // `skip_list_new`
  if (!list) abort();

  while (lvl >= 0) {
    if (n->key < key) {
      prev[lvl] = n;
      n = n->next[lvl];
    } else {
      if (n->key == key) {
        target = n;
        prev[lvl]->next[lvl] = n->next[lvl];
      }

      n = prev[lvl--];
    }
  }

  // till now, prev's all levels' next pointers point to the deleting node's
  // next node and target point to the deleting node(if we found it), since no
  // pointers point to it now, we can just free it
  if (target) free(target);
}

/* delimiter */

void skip_list_dump(struct skip_list *list) {
  printf("skip list:\n");

  int lvl = SKIP_LIST_MAX_LEVEL - 1;
  while (lvl >= 0) {
    struct skip_list_node *n = list->head.next[lvl];

    printf("level %d: ", lvl);
    while (n != &list->tail && n->next[lvl]) {
      if (n->next[lvl] == &list->tail)
        printf("%d", n->key);
      else
        printf("%d->", n->key);
      n = n->next[lvl];
    }
    printf("\n");

    lvl--;
  }
}

int main() {
  struct skip_list *list = skip_list_new();
  insert(list, 1);
  insert(list, 5);
  insert(list, 10);
  insert(list, 15);
  insert(list, 20);
  insert(list, 3);
  insert(list, 36);
  insert(list, 16);
  insert(list, 160);
  insert(list, 130);
  insert(list, 135);
  insert(list, 230);
  insert(list, 530);
  insert(list, 234);
  insert(list, 720);
  insert(list, 150);
  insert(list, 190);
  insert(list, 133);
  insert(list, 30);
  delete (list, 135);
  skip_list_dump(list);
}
