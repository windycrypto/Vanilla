#include <stdio.h>
#include <stdlib.h>

#include "include/avltree2.h"

/*
 * AVL tree balance factor version
 */

/*
 * \return new root
 */
struct avl2_node *rotate(struct avl2_node *root, int dir) {
  struct avl2_node *tmp;

  /* dir != 0, clockwise */
  if (dir) {
    tmp = root->left;
    root->left = tmp->right;
    tmp->right = root;
  } else {
    tmp = root->right;
    root->right = tmp->left;
    tmp->left = root;
  }

  /* must first re-caculate root's height */
  tmp->factor = root->factor = 0;

  return tmp;
}

/*
 * if left subtree's height is 1 more greater than right subtree, and
 * the root's val of left subtree greater than inserted val, then
 * unbalanced condition caused by new node inserted into left subtree's
 * left child
 *
 * Other cases can be deduced in the same way
 */
struct avl2_node *avl2_insert(struct avl2_node *root, int val) {
  struct avl2_node *tmp;
  int h1, h2;

  if (!root) {
    root = malloc(sizeof *root);
    root->val = val;
    root->factor = 0;
    root->left = root->right = NULL;
  } else if (root->val > val) {
    tmp = root->left;
    h1 = tmp ? tmp->factor : -1;
    root->left = avl2_insert(root->left, val);
    h2 = root->left->factor;

    if (!tmp || (root->left == tmp && h2 && h2 != h1)) {
      if (root->factor == 1) {
        if (root->left->factor == 1) {
          /* single rotate */
          root = rotate(root, 1);
        } else if (root->left->factor == -1) {
          /* double rotate */
          if (root->left->right->factor == 1) {
            root->left = rotate(root->left, 0);
            root = rotate(root, 1);
            /* correct factor */
            root->right->factor = -1;
          } else if (root->left->right->factor == -1) {
            root->left = rotate(root->left, 0);
            root = rotate(root, 1);
            /* correct factor */
            root->left->factor = 1;
          } else {
            root->left = rotate(root->left, 0);
            root = rotate(root, 1);
          }
        }
      } else {
        ++root->factor;
      }
    }
  } else if (root->val < val) {
    tmp = root->right;
    h1 = tmp ? tmp->factor : -1;
    root->right = avl2_insert(root->right, val);
    h2 = root->right->factor;

    if (!tmp || (tmp == root->right && h2 && h2 != h1)) {
      if (root->factor == -1) {
        if (root->right->factor == -1) {
          /* single rotate */
          root = rotate(root, 0);
        } else if (root->right->factor == 1) {
          /* double rotate */
          if (root->right->left->factor == 1) {
            root->right = rotate(root->right, 1);
            root = rotate(root, 0);

            root->right->factor = -1;
          } else if (root->right->left->factor == -1) {
            root->right = rotate(root->right, 1);
            root = rotate(root, 0);
            root->left->factor = 1;
          } else {
            root->right = rotate(root->right, 1);
            root = rotate(root, 0);
          }
        }
      } else {
        --root->factor;
      }
    }
  } /* val is already in the tree, do nothing */

done:
  return root;
}

struct avl2_node *avl2_find(struct avl2_node *root, int val) {
  if (!root)
    return NULL;
  else if (root->val > val)
    return avl2_find(root->left, val);
  else if (root->val < val)
    return avl2_find(root->right, val);
  else
    return root;
}

struct avl2_node *avl2_remove(struct avl2_node *root, int val) {
  struct avl2_node *tmp;
  int h1, h2, flag;

  if (!root)
    ;
  else if (root->val > val) {
    if (root->left) {
      tmp = root->left;
      h1 = root->left->factor;
      root->left = avl2_remove(root->left, val);
      if (root->left) h2 = root->left->factor;

      /* single child, (note, double child case has been converted to
       * single child) */
      /*
       * 得益于我们采取的删除节点的方式, 节点的删除只会发生在叶子节点,
       * 因此 这里 root->left == NULL 就意味着叶子节点被删除了
       */
      if (!root->left /* 叶子节点的回溯 */
          || (tmp == root->left && !h2 &&
              h2 != h1) /* 子树未发生旋转, 子树根平衡因子由非 0 变为 0 */
          || (tmp != root->left &&
              !root->left->factor)) /* 子树发生了旋转, 子树新根平衡因子为
                                             0 (暗示子树高度发生了变化) */
      {
        if (root->factor == -1) {
          if (root->right->factor != 1) {
            /* single rotate */
            if (root->right->factor == 0) {
              root = rotate(root, 0);
              /* correct new root's factor */
              root->factor = 1;
              root->left->factor = -1;
            } else { /* root->right->factor == -1 */
              root = rotate(root, 0);
            }
          } else {
            /* double rotate */
            if (root->right->left->factor == 1) {
              root->right = rotate(root->right, 1);
              root = rotate(root, 0);
              root->right->factor = -1;
            } else if (root->right->left->factor == -1) {
              root->right = rotate(root->right, 1);
              root = rotate(root, 0);
              root->left->factor = 1;
            } else {
              root->right = rotate(root->right, 1);
              root = rotate(root, 0);
            }
          }
        } else {
          --root->factor;
        }
      }
    }
  } else if ((root->val < val) || root->right) {
    if (root->right) {
      if (root->val < val) {
        tmp = root->right;
        h1 = tmp->factor;
        root->right = avl2_remove(root->right, val);
        if (root->right) h2 = root->right->factor;

      } else {
        tmp = root->right;
        h1 = tmp->factor;

        while (tmp->left) tmp = tmp->left;
        root->val = tmp->val;

        tmp = root->right;
        root->right = avl2_remove(root->right, root->val);
        if (root->right) h2 = root->right->factor;
      }

      if (!root->right /* 叶子节点的回溯 */
          || (tmp == root->right && !h2 &&
              h2 != h1) /* 子树未发生旋转, 子树根平衡因子由非 0 变为 0 */
          || (tmp != root->right &&
              !root->right->factor)) /* 子树发生了旋转, 子树新根平衡因子为
                                                  0 (暗示子树高度发生了变化) */
      {
        if (root->factor == 1) {
          if (root->left->factor != -1) {
            /* single rotate */
            if (root->left->factor == 0) {
              root = rotate(root, 1);
              /* correct new root's factor */
              root->factor = -1;
              root->right->factor = 1;
            } else { /* root->left->factor == 1 */
              root = rotate(root, 1);
            }
          } else {
            /* double rotate */
            if (root->left->right->factor == -1) {
              root->left = rotate(root->left, 0);
              root = rotate(root, 1);
              /* correct new root's factor */
              root->left->factor = 1;
            } else if (root->left->right->factor == 1) {
              root->left = rotate(root->left, 0);
              root = rotate(root, 1);
              /* correct new root's factor */
              root->right->factor = -1;
            } else {
              root->left = rotate(root->left, 0);
              root = rotate(root, 1);
            }
          }
        } else {
          ++root->factor;
        }
      }
    }
  } else {
    tmp = root->left;
    free(root);
    root = tmp;
  }

  return root;
}
