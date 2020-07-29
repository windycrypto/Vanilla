#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "include/queue.h"
#include "include/rbtree.h"

int rbtree_depth(struct rb_node *root) {
  int d1, d2;

  if (!root)
    return -1;
  else {
    d1 = rbtree_depth(root->left) + 1;
    d2 = rbtree_depth(root->right) + 1;
    return d1 > d2 ? d1 : d2;
  }
}

/*
 * print a tree in leetcode format, this method only applies for printing a
 * balanced tree. Printing unbalanced tree may end up with printing excessive
 * "nil"s
 */
void rbtree_dump(struct rb_node *root) {
  int mdepth, fullcount, count = 0;

  queue_t *queue = create_queue(free);

  mdepth = rbtree_depth(root);
  fullcount = (2 << mdepth) - 1;

  enqueue(queue, root);
  while (queue->size) {
    root = (struct rb_node *)dequeue(queue);

    count++;

    if (!root) {
      if (count <= fullcount) printf(",nil");
    } else {
      if (count > 1) printf(",");

      printf("%d%c", root->val, root->color ? 'b' : 'r');

      enqueue(queue, root->left);
      enqueue(queue, root->right);
    }

    if (queue->size < 1) printf("\n");
  }
}

/*
 * red node's children must be black, if not, return false(0)
 */
int rb_color_check(struct rb_node *root) {
  if (!root)
    return 1;
  else if (rb_node_red(root) &&
           (rb_node_red(root->left) || rb_node_red(root->right)))
    return 0;
  else
    return rb_color_check(root->left) && rb_color_check(root->right);
}

/*
 * root to all leaf nodes got the same number of black nodes, if it is,
 * return the black nodes number, otherwise return 0(false)
 *
 * this is a recursion mean based on the fact that for a balanced red-black
 * tree black nodes should be equal on left tree and right tree
 */
int rb_balance_check(struct rb_node *root) {
  int num;

  // treat null node as black node, so we won't return 0 on an empty tree
  if (!root) return 1;

  if ((num = rb_balance_check(root->left)) != rb_balance_check(root->right))
    return 0;

  if (rb_node_black(root)) num += 1;

  return num;
}

/*
 * check the validation of the red-black tree, i.e. meets these constraints
 *
 *  1. root is black
 *  2. red node's children must be black
 *  3. root to all leaf nodes got the same number of black nodes
 */
int rb_valid(struct rb_node *root) {
  return rb_node_black(root) && rb_color_check(root) && rb_balance_check(root);
}

// compile with `clang rbtree.c queue.c util.c`
int main() {
  struct rb_node *root = NULL;
  assert(rb_valid(root));
  root = rb_insert(root, 9);
  assert(rb_valid(root));
  root = rb_insert(root, 10);
  assert(rb_valid(root));
  root = rb_insert(root, 18);
  assert(rb_valid(root));
  root = rb_insert(root, 20);
  assert(rb_valid(root));
  root = rb_insert(root, 21);
  assert(rb_valid(root));
  root = rb_insert(root, 22);
  assert(rb_valid(root));
  root = rb_insert(root, 24);
  assert(rb_valid(root));
  root = rb_insert(root, 25);
  assert(rb_valid(root));
  root = rb_insert(root, 26);
  assert(rb_valid(root));
  root = rb_insert(root, 30);
  assert(rb_valid(root));
  root = rb_insert(root, 31);
  root = rb_insert(root, 35);
  root = rb_insert(root, 36);
  root = rb_insert(root, 40);
  root = rb_insert(root, 45);
  root = rb_insert(root, 55);
  root = rb_insert(root, 55);
  root = rb_insert(root, 65);
  root = rb_insert(root, 75);

  root = rb_remove(root, 25);
  assert(rb_valid(root));
}
