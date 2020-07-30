#ifndef AVL_TREE2_H_
#define AVL_TREE2_H_

struct avl2_node {
  int val;
  int factor;
  struct avl2_node *left;
  struct avl2_node *right;
};

struct avl2_node *avl2_insert(struct avl2_node *root, int val);
struct avl2_node *avl2_find(struct avl2_node *root, int val);
struct avl2_node *avl2_remove(struct avl2_node *root, int val);

#endif
