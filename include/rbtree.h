#ifndef RBTREE_H_
#define RBTREE_H_

#define RED 0
#define BLACK 1

#define rb_node_red(n) (n && n->color == RED)
#define rb_node_black(n) (!rb_node_red(n))

struct rb_node {
  int val;
  unsigned int color : 1;  // 0: red
                           // 1: black
  struct rb_node *left;
  struct rb_node *right;
};

struct rb_node *rb_insert(struct rb_node *root, int val);
struct rb_node *rb_remove(struct rb_node *root, int val);
struct rb_node *rb_find(struct rb_node *root, int val);

#endif
