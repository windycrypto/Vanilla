#include <stdio.h>
#include <stdlib.h>

struct TreeNode {
    int val;
    int height;
    struct TreeNode *left;
    struct TreeNode *right;
};

/*
 * Get height of node
 */
static inline int _h(struct TreeNode *node) { return node ? node->height : -1; }

/*
 * Caculate height of node, this function will update node's height
 * internally, however, this doesn't prevent you updating it using the
 * return value
 *
 * \return new height
 */
int height(struct TreeNode *root) {
    int h1, h2;

    h1 = _h(root->left);
    h2 = _h(root->right);

#define MAX(h1, h2) ((h1) > (h2) ? (h1) : (h2))
    root->height = MAX(h1, h2) + 1;
    return root->height;
}

/*
 * \return new root
 */
struct TreeNode *rotate(struct TreeNode *root, int dir) {
    struct TreeNode *tmp;

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
    root->height = height(root);
    tmp->height = height(tmp);

    return tmp;
}

/*
 * if left subtree's height is 1 more greater than right subtree, and
 * the root's val of left subtree greater than inserted val, then unbalanced
 * condition caused by new node inserted into left subtree's left child
 *
 * Other cases can be deduced in the same way
 */
struct TreeNode *insert(struct TreeNode *root, int val) {
    if (!root) {
        root = malloc(sizeof *root);
        root->val = val;
        root->left = root->right = NULL;
    } else if (root->val > val) {
        root->left = insert(root->left, val);

        if (_h(root->left) - _h(root->right) > 1) {
            if (root->left->val > val) {
                root = rotate(root, 1);
            } else {
                root->left = rotate(root->left, 0);
                root = rotate(root, 1);
            }
        }
    } else if (root->val < val) {
        root->right = insert(root->right, val);

        if (_h(root->left) - _h(root->right) < -1) {
            if (root->right->val < val) {
                root = rotate(root, 0);
            } else {
                root->right = rotate(root->right, 1);
                root = rotate(root, 0);
            }
        }
    } /* val is already in the tree, do nothing */

    root->height = height(root);

    return root;
}

struct TreeNode *find(struct TreeNode *root, int val) {
    if (!root)
        return NULL;
    else if (root->val > val)
        return find(root->left, val);
    else if (root->val < val)
        return find(root->right, val);
    else
        return root;
}

struct TreeNode *delete (struct TreeNode *root, int val) {
    struct TreeNode *tmp;

    if (!root)
        ;
    else if (root->val > val) {
        root->left = delete (root->left, val);
        if (_h(root->left) - _h(root->right) < -1) {
            if (_h(root->right) - _h(root->right->right) == 1) {
                root = rotate(root, 0);
            } else {
                root->right = rotate(root->right, 1);
                root = rotate(root, 0);
            }
        }
    } else if (root->val < val || (root->left && root->right)) {
        if (root->val < val) {
            root->right = delete (root->right, val);
        } else {
            tmp = root->right;
            while (tmp->left) tmp = tmp->left;
            root->val = tmp->val;
            root->right = delete (root->right, root->val);
        }

        if (_h(root->left) - _h(root->right) > 1) {
            if (_h(root->left) - _h(root->left->left) == 1) {
                root = rotate(root, 1);
            } else {
                root->left = rotate(root->left, 0);
                root = rotate(root, 1);
            }
        }
    } else if (root->left || root->right) {
        tmp = root->left ? root->left : root->right;
        free(root);
        root = tmp;
    } else {
        free(root);
        root = NULL;
    }

    /* If the deleted is an orphan node */
    if (root) root->height = height(root);

    return root;
}

int main() {}
