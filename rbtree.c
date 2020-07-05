#include <stdio.h>
#include <stdlib.h>

/*
 * Print the tree using breadth-first search, this will show tree in the same
 * format with leetcode
 */

/*
 * A very crude queue implementation
 */
struct rb_node;
typedef struct queue {
    struct rb_node *val;
    struct queue *next;
} Queue;

void queueCreate(Queue **queue) { *queue = NULL; }

void enqueue(Queue **queue, struct rb_node *c) {
    Queue *tmp, *p;

    tmp = malloc(sizeof *tmp);
    tmp->val = c;
    tmp->next = NULL;

    if (!*queue) {
        *queue = tmp;
        return;
    }

    for (p = *queue; p->next; p = p->next)
        ;

    p->next = tmp;
}

void dequeue(Queue **queue, struct rb_node **c) {
    Queue *tmp;

    if (!*queue) return;

    tmp = *queue;
    *c = tmp->val;
    *queue = tmp->next;
    free(tmp);
}

int queueEmpty(Queue **queue) { return *queue ? 0 : 1; }

#define RED 0
#define BLACK 1

#define rb_node_red(n) (n && n->color == RED)
#define rb_node_black(n) (!rb_node_red(n))

struct rb_node {
    int val;
    unsigned int color : 1; // 0: red
                            // 1: black
    struct rb_node *left;
    struct rb_node *right;
};

/*
 * left rotation is also counter clockwise rotation
 *
 * \return new root
 */
static inline struct rb_node *_rotate_left(struct rb_node *n) {
    struct rb_node *tmp = n->right;
    n->right = tmp->left;
    tmp->left = n;
    return tmp;
}

/*
 * right rotation is also clockwise rotation
 *
 * \return new root
 */
static inline struct rb_node *_rotate_right(struct rb_node *n) {
    struct rb_node *tmp = n->left;
    n->left = tmp->right;
    tmp->right = n;
    return tmp;
}

/*
 * node color is set to RED by default since we are probably only use this function on inserting
 * new node
 */
static inline struct rb_node *_new_rb_node(int val) {
    struct rb_node *n = calloc(1, sizeof *n);
    n->color = RED;
    n->val = val;
    return n;
}

/*
 * \return the new root
 */
struct rb_node *insert(struct rb_node *root, int val) {
    // inserting into an empty rbtree
    if (!root) {
        root = _new_rb_node(val);
        root->color = BLACK;
        goto done_first_node;
    }

    // go down to find the inserting point, meanwhile keep tracking of its parent,
    // grandparent and great-grandparent
    struct rb_node *n = root, *P = NULL, *GP = NULL, *GGP = NULL;
    while (n) {

        // top-down correlation, if a black node has two red children, flip them
        if (n && rb_node_black(n) && rb_node_red(n->left) && rb_node_red(n->right)) {
            n->color = RED;
            n->left->color = n->right->color = BLACK;

            if (rb_node_red(P)) {
                struct rb_node *nr; // represents the new root of the subtree after rotation

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
                    } else {
                        nr = _rotate_left(GP);
                    }
                }

                nr->color = BLACK;
                GP->color = RED;
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
        goto done_parent_black;
    }

    struct rb_node *nr; // represents the new root of the subtree after rotation

    // reaching here means parent is red, continue to check uncle's color
    // if uncle is black, do rotation and color flipping
    // refer to the *Figure 12.10* of *Data Structures and Algorithm Analysis in C*
    //
    //
    /*
     * Case - inserting node's uncle is black
     *
     *         G(b)      G(b)      G            G
     *        /         /           \            \
     *       P(r)  or  P(r)   or     P     or     P
     *      /           \             \          /
     *     n(r)          n(r)           new      new
     */
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
        goto done_uncle_black;
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
        goto done_uncle_black;
    }

done_uncle_black:
    GP->color = RED;
    nr->color = BLACK;
    if (!GGP)
        root = nr;
    else if (GGP->left == GP)
        GGP->left = nr;
    else
        GGP->right = nr;
done_parent_black:
done_first_node:
    return root;
}

struct rb_node *find(struct rb_node *root, int val) {
    if (!root)
        return NULL;
    else if (root->val > val)
        return find(root->left, val);
    else if (root->val < val)
        return find(root->right, val);
    else
        return root;
}

struct rb_node *delete (struct rb_node *root, int val) {

    if (!root) return root;

    // go down to find the target node, meanwhile do top-down correlation
    //
    // n, used to travel down the rbtree
    // s, represents the current node's sibling
    // P, represents the parent
    // GP, represents the grandparent
    // GGP, represents the great grandparent
    struct rb_node *n = root, *s = NULL, *P = NULL, *GP = NULL;
    while (n) {
        // do top-down correlation
        struct rb_node *nr;

        if (rb_node_black(P)) {
            if (rb_node_red(s)) { // case 4 - sibling is red
                if (P->left == s) {
                    nr = _rotate_right(P);
                    s = P->left;
                } else {
                    nr = _rotate_left(P);
                    s = P->right;
                }

                nr->color = BLACK;
                P->color = RED;

                if (!GP)
                    root = nr;
                else if (GP->left == P)
                    GP->left = nr;
                else
                    GP->right = nr;
            } else { // either n is red or not, continue going down
                goto next;
            }
        }

        if (rb_node_red(P)) { // if P is red, its two children must be black
            if (rb_node_red(n->left) || rb_node_red(n->right)) {
                goto next;
            }

            // case 1 - if its two children also has no red children, flip them.
            if (rb_node_black(s->left) && rb_node_black(s->right)) {
                P->color = BLACK;
                n->color = s->color = RED;
            } else if (rb_node_red(s->right)) { // case 2 - include the case of
                                                // sibling's left child is red
                if (s == P->right) {
                    nr = _rotate_left(P);
                    n->color = RED;
                    P->color = BLACK;
                    s->right->color = BLACK;
                    nr->color = RED;
                } else {
                    P->left = _rotate_left(s);
                    nr = _rotate_right(P);
                    n->color = RED;
                    P->color = BLACK;
                    nr->color = RED;
                }

                if (!GP)
                    root = nr;
                else if (GP->left == P)
                    GP->left = nr;
                else
                    GP->right = nr;

            } else { // case 3
                if (s == P->right) {
                    P->right = _rotate_right(s);
                    nr = _rotate_left(P);
                    n->color = RED;
                    P->color = BLACK;
                    nr->color = RED;
                } else {
                    nr = _rotate_right(P);
                    n->color = RED;
                    P->color = BLACK;
                    nr->color = RED;
                    s->left->color = BLACK;
                }

                if (!GP)
                    root = nr;
                else if (GP->left == P)
                    GP->left = nr;
                else
                    GP->right = nr;
            }
            // if (P == root) n->color = BLACK; // need to color root back
        }

    next:

        if (n->val > val) {
            GP = P, P = n;
            s = n->right;
            n = n->left;
        } else if (n->val < val) {
            GP = P, P = n;
            s = n->left;
            n = n->right;
        } else if (n->right) { // swap with the smallest node of right subtree
            struct rb_node *tmp = n->right;
            while (tmp->left)
                tmp = tmp->left;
            // swap node value. Note, don't swap color
            val = n->val = tmp->val;

            GP = P, P = n;
            s = n->left;
            n = n->right;
        } else if (n->left) { // swap with the largest node of left subtree
            struct rb_node *tmp = n->left;
            while (tmp->right)
                tmp = tmp->right;
            val = n->val = tmp->val;

            GP = P, P = n;
            s = n->right;
            n = n->left;
        } else {
            // already the leaf node
            break;
        }
    }

    // n is the target left node, if n is already red, remove it directly
    if (rb_node_red(n)) {
        if (P->left == n)
            P->left = NULL;
        else
            P->right = NULL;
        free(n);

        return root;
    }

    // NOTE normally we will never reach here

    return root;
}

int maxdepth(struct rb_node *root) {
    int d1, d2;

    if (!root)
        return -1;
    else {
        d1 = maxdepth(root->left) + 1;
        d2 = maxdepth(root->right) + 1;
        return d1 > d2 ? d1 : d2;
    }
}

/*
 * print a tree in leetcode format, this method only applies for printing a balanced tree.
 * Printing unbalanced tree may end up with printing excessive "nil"s
 */
void print(struct rb_node *root) {
    Queue *queue;
    int mdepth, fullcount, count;
    struct rb_node *node;

    queueCreate(&queue);

    mdepth = maxdepth(root);
    fullcount = (2 << mdepth) - 1;

    count = 0;

    enqueue(&queue, root);

    while (!queueEmpty(&queue)) {
        dequeue(&queue, &node);

        count++;

        if (!node) {
            if (count <= fullcount) printf(",nil");
        } else {
            if (count > 1) printf(",");

            printf("%d%c", node->val, node->color ? 'b' : 'r');

            enqueue(&queue, node->left);
            enqueue(&queue, node->right);
        }

        if (queueEmpty(&queue)) printf("\n");
    }
}

int main() {
    struct rb_node *root = NULL;
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
    print(root);

    root = delete (root, 31);
    print(root);
}
