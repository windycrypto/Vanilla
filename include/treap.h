#ifndef TREAP_H_
#define TREAP_H_

struct treap_node {
    int key;
    int priority;
    struct treap_node *left;
    struct treap_node *right;
};

#endif /* TREAP_H_ */
