#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "include/avltree2.h"
#include "include/queue.h"

static int _depth(struct avl2_node *root) {
  int d1, d2;

  if (!root)
    return -1;
  else {
    d1 = _depth(root->left) + 1;
    d2 = _depth(root->right) + 1;
    return d1 > d2 ? d1 : d2;
  }
}

/*
 * print a tree in leetcode format, this method only applies for printing a
 * balanced tree. Printing unbalanced tree may end up with printing excessive
 * "nil"s
 */
void rb_dump(struct avl2_node *root) {
  int mdepth, fullcount, count = 0;

  queue_t *queue = create_queue(free);

  mdepth = _depth(root);
  fullcount = (2 << mdepth) - 1;

  enqueue(queue, root);
  while (queue->size) {
    root = (struct avl2_node *)dequeue(queue);

    count++;

    if (!root) {
      if (count <= fullcount) printf(",nil");
    } else {
      if (count > 1) printf(",");

      printf("%d%d", root->val, root->factor ? 'b' : 'r');

      enqueue(queue, root->left);
      enqueue(queue, root->right);
    }

    if (queue->size < 1) printf("\n");
  }
}

int avl_check_height(struct avl2_node *root) {
  // in contrast of returnning -1 on NULL node in avl tree implementation,
  // we return 0 during checking so we can rely on -1 to determine whether
  // the tree is valid or not recursively
  if (!root) return 0;

  int h1, h2;

  h1 = avl_check_height(root->left);
  h2 = avl_check_height(root->right);

  // either h1 or h2 is -1 indicating it is unbalanced in subtree, so just
  // return -1 forward to upper level
  if (h1 == -1 || h2 == -1 || h1 - h2 > 1 || h1 - h2 < -1) return -1;

  return h1 > h2 ? h1 + 1 : h2 + 1;
}

/*
 * check the validation of the avl tree, i.e. the discrepancy of the height
 * of left and right subtree should be less or queal than 1, recursively
 */
int avl_valid(struct avl2_node *root) { return avl_check_height >= 0; }

// compile with `clang rbtree.c queue.c util.c`
int main() {
  int test_data[] = {
      9,  30, 50, 1,  12, 60, 28, 99,  101, 34, 55, 14, 19,
      87, 49, 33, 28, 65, 70, 9,  110, 200, 15, 45, 32, 8,
  };

  struct avl2_node *root = NULL;

  // test inserting
  for (int i = 0; i < sizeof test_data / sizeof test_data[0]; i++) {
    root = avl2_insert(root, test_data[i]);
    assert(avl_valid(root));
    assert(avl2_find(root, test_data[i]));
  }

  // test deleting
  for (int i = 0; i < sizeof test_data / sizeof test_data[0]; i++) {
    root = avl2_remove(root, test_data[i]);
    assert(avl_valid(root));
    assert(!avl2_find(root, test_data[i]));
  }
  printf("success!\n");
}
