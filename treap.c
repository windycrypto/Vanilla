#include <stdio.h>
#include <stdlib.h>

#include "include/queue.h"
#include "include/treap.h"


/*
 * left rotation is also counter clockwise rotation
 *
 * \return new root
 */
static inline struct treap_node *_rotate_left(struct treap_node *n) {
    struct treap_node *tmp = n->right;
    n->right = tmp->left;
    tmp->left = n;
    return tmp;
}

/*
 * right rotation is also clockwise rotation
 *
 * \return new root
 */
static inline struct treap_node *_rotate_right(struct treap_node *n) {
    struct treap_node *tmp = n->left;
    n->left = tmp->right;
    tmp->right = n;
    return tmp;
}

static inline struct treap_node *_treap_node_new(int key) {
    struct treap_node *n = calloc(1, sizeof *n);
    n->priority = random() & 0xFFFF;
    n->key = key;
    return n;
}

struct treap_node *insert(struct treap_node *root, int key) {
    if (!root)
        return _treap_node_new(key);

    if (root->key > key) {
        root->left = insert(root->left, key);
        if (root->priority > root->left->priority)
            root = _rotate_right(root);
    } else if (root->key < key) {
        root->right = insert(root->right, key);
        if (root->priority > root->right->priority)
            root = _rotate_left(root);
    } // do nothing if the inserting node exists

    return root;
}

struct treap_node *delete(struct treap_node *root, int key) {
    if (!root);
    else if (root->key > key) {
        root->left = delete(root->left, key);
    } else if (root->key < key) {
        root->right = delete(root->right, key);
    } else if (root->right) {
        struct treap_node *tmp = root->right;
        while (tmp->left) tmp = tmp->left;
        root->key = tmp->key, tmp->key = key;
        root->right = delete(root->right, key);
    } else {
        free(root);
        root = root->left;
    }
    return root;
}

struct treap_node *find(struct treap_node *root, int key) {
    if (!root || root->key == key) return root;

    if (root->key > key)
        return find(root->left, key);
    else 
        return find(root->right, key);
}


int depth(struct treap_node *root) {
    int d1, d2;

    if (!root)
        return -1;
    else {
        d1 = depth(root->left) + 1;
        d2 = depth(root->right) + 1;
        return d1 > d2 ? d1 : d2;
    }
}

/*
 * print a tree in leetcode format, this method only applies for printing a balanced tree.
 * Printing unbalanced tree may end up with printing excessive "nil"s
 */
void treap_dump(struct treap_node *root) {
    int mdepth, fullcount, count = 0;

    queue_t *queue = create_queue(free);

    mdepth = depth(root);
    fullcount = (2 << mdepth) - 1;

    enqueue(queue, root);
    while (queue->size) {
        root = (struct treap_node *)dequeue(queue);

        count++;

        if (!root) {
            if (count <= fullcount) printf(",nil");
        } else {
            if (count > 1) printf(",");

            printf("%d(%d)", root->key, root->priority);

            enqueue(queue, root->left);
            enqueue(queue, root->right);
        }

        if (queue->size < 1) printf("\n");
    }
}

int main() {
    struct treap_node *root = NULL;
    root = insert(root, 9);
    root = insert(root, 10);
    root = insert(root, 18);
    root = insert(root, 20);
    root = insert(root, 21);
    root = insert(root, 22);
    root = insert(root, 24);
    root = insert(root, 25);
    root = insert(root, 26);
    root = insert(root, 30);
    root = insert(root, 31);
    root = insert(root, 35);
    root = insert(root, 36);
    root = insert(root, 40);
    root = insert(root, 45);
    root = insert(root, 55);
    root = insert(root, 55);
    root = insert(root, 65);
    root = insert(root, 75);
    treap_dump(root);

    root = delete (root, 25);
    treap_dump(root);
}
