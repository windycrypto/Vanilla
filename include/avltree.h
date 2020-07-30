#ifndef AVL_TREE_H_
#define AVL_TREE_H_

struct avl_node {
  int val;
  int height;
  struct avl_node *left;
  struct avl_node *right;
};

struct avl_node *avl_insert(struct avl_node *root, int val);
struct avl_node *avl_find(struct avl_node *root, int val);
struct avl_node *avl_remove(struct avl_node *root, int val);

#endif
