#include <stdlib.h>

#include "include/rbtree.h"

/*
 * This is a vanilla red-black tree implementation described in
 * *Data Strucutrues and Algorithm Analysis in C* written by Weiss, which
 * I think is the most excellent illustration for red-black tree.
 */

#define rb_update_link_removal(root, GP, P, nr)    \
  do {                                             \
    if (!GP) {                                     \
      root = nr;                                   \
      if (root->color == RED) root->color = BLACK; \
    } else if (GP->left == P)                      \
      GP->left = nr;                               \
    else                                           \
      GP->right = nr;                              \
  } while (0)

#define rb_single_left_rotate() \
  do {                          \
    nr = _rotate_left(P);       \
    n->color = RED;             \
    P->color = BLACK;           \
    nr->color = RED;            \
    s->right->color = BLACK;    \
  } while (0)

#define rb_single_right_rotate() \
  do {                           \
    nr = _rotate_right(P);       \
    n->color = RED;              \
    P->color = BLACK;            \
    nr->color = RED;             \
    s->left->color = BLACK;      \
  } while (0)

#define rb_left_right_rotate() \
  do {                         \
    P->left = _rotate_left(s); \
    nr = _rotate_right(P);     \
    n->color = RED;            \
    P->color = BLACK;          \
    nr->color = RED;           \
  } while (0)

#define rb_right_left_rotate()   \
  do {                           \
    P->right = _rotate_right(s); \
    nr = _rotate_left(P);        \
    n->color = RED;              \
    P->color = BLACK;            \
    nr->color = RED;             \
  } while (0)

/*
 * left rotation is counter clockwise rotation
 */
static inline struct rb_node *_rotate_left(struct rb_node *n) {
  struct rb_node *tmp = n->right;
  n->right = tmp->left;
  tmp->left = n;
  return tmp;
}

/*
 * right rotation is clockwise rotation
 */
static inline struct rb_node *_rotate_right(struct rb_node *n) {
  struct rb_node *tmp = n->left;
  n->left = tmp->right;
  tmp->right = n;
  return tmp;
}

/*
 * node color is set to RED by default since we are probably only use this
 * function on inserting new node
 */
static inline struct rb_node *_new_rb_node(int val) {
  struct rb_node *n = calloc(1, sizeof *n);
  n->color = RED;
  n->val = val;
  return n;
}

/*
 * return the new root
 */
struct rb_node *rb_insert(struct rb_node *root, int val) {
  // inserting into an empty rbtree
  if (!root) {
    root = _new_rb_node(val);
    root->color = BLACK;
    return root;
  }

  // go down to find the inserting point, meanwhile keep tracking of its parent,
  // grandparent and great-grandparent
  struct rb_node *n = root, *nr = NULL, *P = NULL, *GP = NULL, *GGP = NULL;
  while (n) {
    // top-down correlation, if a black node has two red children, flip them
    if (n && rb_node_black(n) && rb_node_red(n->left) &&
        rb_node_red(n->right)) {
      n->color = RED;
      n->left->color = n->right->color = BLACK;

      if (rb_node_red(P)) {
        if (P->left == n) {
          if (GP->left == P)
            nr = _rotate_right(GP);
          else {
            GP->right = _rotate_right(P);
            nr = _rotate_left(GP);
          }
        } else {
          if (GP->left == P) {
            GP->left = _rotate_left(P);
            nr = _rotate_right(GP);
          } else
            nr = _rotate_left(GP);
        }

        GP->color = RED;
        nr->color = BLACK;
        if (!GGP)
          root = nr;
        else if (GGP->left == GP)
          GGP->left = nr;
        else
          GGP->right = nr;
      }
      if (n == root) n->color = BLACK;
    }

    GGP = GP, GP = P, P = n;

    // if the inserting value already exists, skip
    if (n->val == val)
      return root;
    else if (n->val > val)
      n = n->left;
    else
      n = n->right;
  }

  // if parent is black, we are done
  if (rb_node_black(P)) {
    n = _new_rb_node(val);
    if (P->val > val)
      P->left = n;
    else
      P->right = n;
    return root;
  }

  // reaching here means parent is red, continue to check uncle's color
  // if uncle is black, do rotation and color flipping
  // refer to the *Figure 12.10* of *Data Structures and Algorithm Analysis in
  // C*
  if (rb_node_black(GP->left)) {
    /*
     * Case - inserting node's uncle is black
     *
     *      G            G
     *       \            \
     *        P     or     P
     *         \          /
     *          new      new
     */
    n = _new_rb_node(val);
    if (P->val < val) {
      P->right = n;
      nr = _rotate_left(GP);
    } else {
      P->left = n;
      GP->right = _rotate_right(P);
      nr = _rotate_left(GP);
    }
  } else if (rb_node_black(GP->right)) {
    /*
     * Case - inserting node's uncle is black
     *
     *         G(b)      G(b)
     *        /         /
     *       P(r)  or  P(r)
     *      /           \
     *     n(r)          n(r)
     */
    n = _new_rb_node(val);
    if (P->val < val) {
      P->right = n;
      GP->left = _rotate_left(P);
      nr = _rotate_right(GP);
    } else {
      P->left = n;
      nr = _rotate_right(GP);
    }
  }

  GP->color = RED;
  nr->color = BLACK;
  if (!GGP)
    root = nr;
  else if (GGP->left == GP)
    GGP->left = nr;
  else
    GGP->right = nr;

  return root;
}

struct rb_node *rb_find(struct rb_node *root, int val) {
  if (!root)
    return NULL;
  else if (root->val > val)
    return rb_find(root->left, val);
  else if (root->val < val)
    return rb_find(root->right, val);
  else
    return root;
}

/*
 */
struct rb_node *rb_remove(struct rb_node *root, int val) {
  // go down to find the target node, meanwhile do top-down correlation
  //
  // n, used to travel down the rbtree
  // s, represents the current node's sibling
  // nr, represents the new root node of the subtree after rotation
  // P, represents the parent
  // GP, represents the grandparent
  struct rb_node *n = root, *s = NULL, *nr = NULL, *P = NULL, *GP = NULL;
  while (n) {
    // do top-down correlation

    if (rb_node_black(P)) {
      if (rb_node_red(n))
        ;  // do nothing and keep going, note we should
           // always check `n` before its sibling
      else if (rb_node_red(s)) {
        if (P->left == s) {
          nr = _rotate_right(P);
          s = P->left;
        } else {
          nr = _rotate_left(P);
          s = P->right;
        }
        nr->color = BLACK;
        P->color = RED;

        rb_update_link_removal(root, GP, P, nr);
        continue;
      } else if (P == root) {
        P->color = RED;
        continue;
      }
    } else if (rb_node_red(P)) {  // P is red n and its sibling must be black
      if (P == root) P->color = BLACK;  // need to color root back

      if (rb_node_red(n->left) || rb_node_red(n->right))
        ;  // keep going down in this case
      else if (rb_node_black(s->left) && rb_node_black(s->right)) {
        P->color = BLACK;
        n->color = s->color = RED;
      } else if (rb_node_red(s->right)) {
        if (s == P->right)
          rb_single_left_rotate();
        else
          rb_left_right_rotate();
        rb_update_link_removal(root, GP, P, nr);
      } else {
        if (s == P->right)
          rb_right_left_rotate();
        else
          rb_single_right_rotate();
        rb_update_link_removal(root, GP, P, nr);
      }
    }

    if (n->val > val)
      GP = P, P = n, s = n->right, n = n->left;
    else if (n->val < val)
      GP = P, P = n, s = n->left, n = n->right;
    else if (n->right) {  // swap with the smallest node of right subtree
      struct rb_node *tmp = n->right;
      while (tmp->left) tmp = tmp->left;
      // swap node value. Note, don't swap color
      val = n->val = tmp->val;

      GP = P, P = n, s = n->left, n = n->right;
    } else if (n->left) {  // swap with the largest node of left subtree
      struct rb_node *tmp = n->left;
      while (tmp->right) tmp = tmp->right;
      val = n->val = tmp->val;

      GP = P, P = n, s = n->right, n = n->left;
    } else
      // already the leaf node
      break;
  }

  // in our implementation, when reaching here n must be red except for only
  // one case: the tree has only a root node, then n is the root thus black
  if (rb_node_red(n)) {
    if (P->left == n)
      P->left = NULL;
    else
      P->right = NULL;
    free(n);
  } else if (n == root) {  // n must be root
    free(n);
    root = NULL;
  } else
    ;  // node not found
  return root;
}
