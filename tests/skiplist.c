#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "include/skiplist.h"

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
  int test_data[] = {
#include "numbers.txt"
  };

  struct skip_list *list = skip_list_new();

  // test inserting
  for (int i = 0; i < sizeof test_data / sizeof test_data[0]; i++) {
    skip_list_insert(list, test_data[i]);
    assert(skip_list_find(list, test_data[i]));
  }

  // test deleting
  for (int i = 0; i < sizeof test_data / sizeof test_data[0]; i++) {
    skip_list_remove(list, test_data[i]);
    assert(!skip_list_find(list, test_data[i]));
  }
  printf("success!\n");
}
