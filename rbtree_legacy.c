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

void queueCreate(Queue **queue)
{
	*queue = NULL;
}

void enqueue(Queue **queue, struct rb_node *c)
{
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

void dequeue(Queue **queue, struct rb_node **c)
{
	Queue *tmp;

	if (!*queue)
		return;

	tmp = *queue;
	*c = tmp->val;
	*queue = tmp->next;
	free(tmp);
}

int queueEmpty(Queue **queue)
{
	return *queue ? 0 : 1;
}

#define RED 0
#define BLACK 1

#define rb_is_red(n) (n->color == RED)
#define rb_is_leaf(n) (!n->left && !n->right)
#define rb_parent(n) (n->parent)

struct rb_node {
	int val;
	int color; // 0: red
		// 1: black
	struct rb_node *parent;
	struct rb_node *left;
	struct rb_node *right;
};

/*
 * \param dir, rotation direction, >0 means clockwise rotation
 *
 * \return new root
 */
struct rb_node *rotate(struct rb_node *n, int dir)
{
	struct rb_node *tmp;

	/* dir != 0, clockwise */
	if (dir) {
		tmp = n->left;
		n->left = tmp->right;
		tmp->right = n;
	} else {
		tmp = n->right;
		n->right = tmp->left;
		tmp->left = n;
	}

	return tmp;
}

/*
 * \return a new root
 */
struct rb_node *insert(struct rb_node *root, int val)
{
	// inserting into an empty rbtree
	if (!root) {
		root = calloc(1, sizeof *root);
		root->parent = NULL;
		root->left = root->right = NULL;
		root->color = BLACK;
		root->val = val;
		return root;
	}

	// go down to find the inserting point
	struct rb_node *nn = root, *parent = NULL, *gparent = NULL,
		       *ggparent = NULL, *gggparent = NULL;
	while (nn) {
		gggparent = ggparent;
		ggparent = gparent;
		gparent = parent;
		parent = nn;

		// if the inserting value already exists, skip
		if (nn->val == val)
			return root;
		else if (nn->val > val)
			nn = nn->left;
		else
			nn = nn->right;
	}

	// allocate the new rb node
	nn = calloc(1, sizeof *root);
	nn->left = nn->right = NULL;
	nn->color = RED;
	nn->val = val;

	// if parent is black, we are done
	if (!rb_is_red(parent)) {
		if (parent->val > val) {
			parent->left = nn;
		} else {
			parent->right = nn;
		}
		return root;
	}

	/*
   * ([Remove me]: all tested)
   *
   * Case 1 - inserting node's uncle doesn't exist
   *
   *         G(b)      G(b)      G            G
   *        /         /           \            \
   *       P(r)  or  P(r)   or     P     or     P
   *      /           \             \          /
   *     n(r)          n(r)           new      new
   *
   * rotate on parent or grandparent and do color flips accordingly, note that
   * when falls in this case G must be black and P must be red
   */
	if (!gparent->left || !gparent->right) {
		struct rb_node *
			nr; // represents the new root of the subtree after rotation
		if (!gparent->left) {
			if (parent->val < val) {
				// bind the new node with current parent temporarily
				parent->right = nn;

				/*
         * Case 1.1
         *
         * G(b)                P(r)                           P(b)
         *  \                /    \                          /    \
         *   P(r)    -->    G(b)  n(r)   -->(color flips)   G(r)  n(r)
         *    \
         *    new(r)
         */
				nr = rotate(gparent, 0);

				nr->color = BLACK;
				gparent->color = RED;
			} else {
				// bind the new node with current parent temporarily
				parent->left = nn;

				/*
         * Case 1.2
         *
         *  G(b)          G(b)           n(r)                  n(b)
         *     \            \           /    \     (flip)     /   \
         *      P(r)  -->   n(r)  -->  G(b)  P(r)  ---->    G(r)  P(r)
         *     /               \
         *   n(r)              P(r)
         */

				gparent->right = rotate(parent, 1);
				nr = rotate(gparent, 0);

				nr->color = BLACK;
				gparent->color = RED;
			}

		} else {
			if (parent->val > val) {
				// bind the new node with current parent temporarily
				parent->left = nn;

				/*
         * Case 1.3
         *
         *     G(b)           P(r)                            P(b)
         *     /             /    \                          /    \
         *   P(r)   -->    n(r)   G(b)  -->(color flips)   n(r)   G(r)
         *   /
         *  n(r)
         */
				nr = rotate(gparent, 1);

				nr->color = BLACK;
				gparent->color = RED;

			} else {
				// bind the new node with current parent temporarily
				parent->right = nn;

				/*
         * Case 1.4
         *
         *   G(b)         G(b)       n(r)                   n(b)
         *   /            /         /    \                 /   \
         *  P(r)  -->   n(r)  -->  P(r)  G(b)  -->(flip)  P(r)  G(r)
         *   \           /
         *   n(r)       P(r)
         */

				gparent->left = rotate(parent, 0);
				nr = rotate(gparent, 1);

				nr->color = BLACK;
				gparent->color = RED;
			}
		}

		if (!ggparent) { // ggparent is NULL means gparent is root
			root = nr;
		} else if (ggparent->left == gparent) {
			ggparent->left = nr;
		} else if (ggparent->right == gparent) {
			ggparent->right = nr;
		}

		return root;
	}

	/*
   * Case 2 - inserting node's uncle is red
   *
   *      G(b)            G(b)              G(b)                G(b)
   *      /  \            /   \            /   \               /    \
   *    P(r)  U(r)  or  P(r)  U(r)   or   U(r)  P(r)    or   U(r)  P(r)
   *    /                 \                      \                  /
   *   n(r)               n(r)                   n(r)              n(r)
   *
   * rotate on parent or grandparent and do color flips accordingly, note that
   * when falls in this case G must be black and P, U must be red
   *
   * U cannot be black or it breaks the red-black tree's constraints
   */
	if (gparent->left == parent) {
		struct rb_node *
			nr; // represents the new root of the subtree after rotation
		if (parent->val > val) {
			// bind the new node with current parent temporarily
			parent->left = nn;

			/*
       * Case 2.1
       *
       *      G(b)              G(r)
       *      /  \      flip    /   \
       *    P(r)  U(r)  --->  P(b)  U(b)
       *    /                  /
       *   n(r)               n(r)
       */

			gparent->color = RED;
			parent->color = BLACK;
			gparent->right->color = BLACK;

			if (!ggparent) { // gparent(G) is the root node
				/*
         * Case 2.1.1
         *
         *      G(r)              G(b)
         *      /  \      flip    /   \
         *    P(b)  U(b)  --->  P(b)  U(b)
         *    /                  /
         *   n(r)               n(r)
         */
				gparent->color = BLACK;
			} else if (!rb_is_red(ggparent)) {
				// ggparent is black, do nothing
				/*
         * Case 2.1.2
         *
         *        GGP(b)
         *        /    \
         *      G(r)
         *      /  \
         *    P(b)  U(b)
         *    /
         *   n(r)
         *
         * here GGP(grand grand parent)'s right substree must have a
         * black node
         */
			} else { // ggparent is red

				if (ggparent->left == gparent) {
					printf("hit\n");
					/*
           * Case 2.1.3.1
           *
           * here GGP(grand grand parent)'s right substree(α) must have
           * one and only one black node
           *
           *
           *      GGP(r)                G(r)                  G(b)
           *      /    \                /   \                 /   \
           *    G(r)    α   rotate    P(b)   GGP(r)  flip    P(b) GGP(r)
           *    /  \        ----->    /      /   \   ----->  /     /   \
           *  P(b)  U(b)             n(r)   U(b)  α         n(r)  U(b) α
           *  /
           * n(r)
           *
           */

					nr = rotate(ggparent, 1);
					nr->color = BLACK;

					if (!gggparent) {
						// ggparent is NULL means gparent is root
						root = nr;
					} else if (gggparent->left ==
						   ggparent) {
						gggparent->left = nr;
					} else if (gggparent->right ==
						   ggparent) {
						gggparent->right = nr;
					}
				} else {
					/*
           * Case 2.1.3.2
           *
           * here GGP(grand grand parent)'s right substree(α) must have
           * one and only one black node
           *
           *
           *   GGP(r)                    G(r)                  G(b)
           *   /   \                    /   \                  /   \
           *  α    G(r)     rotate    GG(r)   U(b)    flip   GG(r) U(b)
           *      /  \      ----->    /  \      \    ----->  / \      \
           *    P(b)  U(b)           α   P(b)   U(b)        α  P(r)   U(b)
           *    /                        /                      /
           *   n(r)                     n(r)                   n(r)
           *
           */

					nr = rotate(ggparent, 0);
					nr->color = BLACK;

					if (!gggparent) {
						// ggparent is NULL means gparent is root
						root = nr;
					} else if (gggparent->left ==
						   ggparent) {
						gggparent->left = nr;
					} else if (gggparent->right ==
						   ggparent) {
						gggparent->right = nr;
					}
				}
			}

		} else {
			// bind the new node with current parent temporarily
			parent->right = nn;

			/*
       * Case 2.2
       *
       *      G(b)               G(b)                  G(r)
       *      /  \      rotate   /   \      flip      /   \
       *    P(r)  U(r)  ----->  n(r)  U(r)  ----->  n(b)  U(b)
       *      \                 /                    /
       *      n(r)             P(r)                P(r)
       */

			/*
      fprintf(stdout,
              "gparent: %p, gparent->left: %p, ggparent: %p, ggparent->val: "
              "%d, ggparent->color: %d, parent: %p, "
              "parent->right: %p, inserting node: %p\n",
              gparent, gparent->left, ggparent, ggparent->val, ggparent->color,
              parent, parent->right, nn);
              */
			nr = rotate(parent, 0);
			gparent->left = nr;

			gparent->color = RED;
			nr->color = BLACK;
			gparent->right->color = BLACK;

			if (!ggparent) { // gparent(G) is the root node
				/*
         * Case 2.2.1
         *
         *      G(r)              G(b)
         *      /  \      flip    /   \
         *    n(b)  U(b)  --->  n(b)  U(b)
         *    /                  /
         *   P(r)               P(r)
         */
				gparent->color = BLACK;
			} else if (!rb_is_red(ggparent)) {
				// ggparent is black, do nothing
				/*
         * Case 2.2.2
         *
         *        GGP(b)
         *        /    \
         *      G(r)
         *      /  \
         *    n(b)  U(b)
         *    /
         *   P(r)
         *
         * here GGP(grand grand parent)'s right substree must have a
         * black node
         */
			} else { // ggparent is red
				if (ggparent->left == gparent) {
					/*
           * Case 2.2.3.1
           *
           * here GGP(grand grand parent)'s right substree(α) must have
           * one and only one black node
           *
           *
           *        GGP(r)                G(r)                  G(b)
           *        /    \                /   \                 /   \
           *      G(r)    α   rotate    n(b)   GGP(r)  flip    n(b) GGP(r)
           *      /  \        ----->    /      /   \   ----->  /     /   \
           *    n(b)  U(b)             P(r)   U(b)  α         P(r)  U(b) α
           *    /
           *   P(r)
           *
           */

					struct rb_node *gggparent =
						rb_parent(ggparent);

					nr = rotate(ggparent, 1);
					nr->color = BLACK;

					if (!gggparent) {
						// ggparent is NULL means gparent is root
						root = nr;
					} else if (gggparent->left ==
						   ggparent) {
						gggparent->left = nr;
					} else if (gggparent->right ==
						   ggparent) {
						gggparent->right = nr;
					}
				} else {
					/*
           * Case 2.2.3.2
           *
           * here GGP(grand grand parent)'s right substree(α) must have
           * one and only one black node
           *
           *
           *     GGP(r)                   G(r)                  G(b)
           *     / \                      /   \                 /   \
           *    α  G(r)       rotate   GGP(r)  U(b)  flip    GGP(r) U(b)
           *      /  \        ----->    / \          ----->  / \
           *    n(b)  U(b)             α  n(b)              α   n(b)
           *    /                         /                     /
           *   P(r)                      P(r)                  P(r)
           *
           */
					struct rb_node *gggparent =
						rb_parent(ggparent);

					nr = rotate(ggparent, 0);
					nr->color = BLACK;

					if (!gggparent) {
						// ggparent is NULL means gparent is root
						root = nr;
					} else if (gggparent->left ==
						   ggparent) {
						gggparent->left = nr;
					} else if (gggparent->right ==
						   ggparent) {
						gggparent->right = nr;
					}
				}
			}
		}
	} else {
		struct rb_node *
			nr; // represents the new root of the subtree after rotation
		if (parent->val > val) {
			// bind the new node with current parent temporarily
			parent->left = nn;

			/*
       * Case 2.3
       *
       *    G(b)                   G(r)
       *    /   \       flip       /  \
       *  U(r)  P(r)    ---->    U(b)  P(b)
       *          \                      \
       *          n(r)                   n(r)
       */
			gparent->color = RED;
			parent->color = BLACK;
			gparent->right->color = BLACK;

			if (!ggparent) { // gparent(G) is the root node
				/*
         * Case 2.3.1
         *
         *      G(r)              G(b)
         *      /  \      flip    /   \
         *    U(b)  P(b)  --->  U(b)  P(b)
         *           \                  \
         *           n(r)               n(r)
         */
				gparent->color = BLACK;
			} else if (!rb_is_red(ggparent)) {
				// ggparent is black, do nothing
				/*
         * Case 2.3.2
         *
         *        GGP(b)
         *        /    \
         *      G(r)
         *      /  \
         *    U(b)  P(b)
         *           \
         *           n(r)
         *
         * here GGP(grand grand parent)'s right substree must have a
         * black node
         */
			} else { // ggparent is red
				if (ggparent->left == gparent) {
					/*
           * Case 2.3.3.1
           *
           * here GGP(grand grand parent)'s right substree(α) must have
           * one and only one black node
           *
           *
           *      GGP(r)                G(r)                  G(b)
           *      /    \                /   \                 /   \
           *    G(r)    α   rotate    U(b)   GGP(r)   flip   U(b) GGP(r)
           *    /  \        ----->           /   \   ----->        /   \
           *  U(b)  P(b)                    P(b)  α               P(b)  α
           *         \                      /                     /
           *         n(r)                  n(r)                  n(r)
           *
           */

					struct rb_node *gggparent =
						rb_parent(ggparent);

					nr = rotate(ggparent, 1);
					nr->color = BLACK;

					if (!gggparent) {
						// ggparent is NULL means gparent is root
						root = nr;
					} else if (gggparent->left ==
						   ggparent) {
						gggparent->left = nr;
					} else if (gggparent->right ==
						   ggparent) {
						gggparent->right = nr;
					}
				} else {
					/*
           * Case 2.3.3.2
           *
           * here GGP(grand grand parent)'s right substree(α) must have
           * one and only one black node
           *
           *
           *  GGP(r)                    G(r)                  G(b)
           *  /  \                     /   \                 /   \
           * α   G(r)     rotate   GGP(r)   P(b)    flip   GGP(r) P(b)
           *    /  \      ----->     / \      \    ----->   / \      \
           *  U(b)  P(b)            α   U(b)  n(r)   P(b)  α  U(b)   n(r)
           *         \
           *         n(r)
           *
           */
					struct rb_node *gggparent =
						rb_parent(ggparent);

					nr = rotate(ggparent, 0);
					nr->color = BLACK;

					if (!gggparent) {
						// ggparent is NULL means gparent is root
						root = nr;
					} else if (gggparent->left ==
						   ggparent) {
						gggparent->left = nr;
					} else if (gggparent->right ==
						   ggparent) {
						gggparent->right = nr;
					}
				}
			}
		} else {
			// bind the new node with current parent temporarily
			parent->right = nn;

			/*
       * Case 2.4
       *
       *      G(b)                G(r)
       *      /  \       flip     /   \
       *    U(r)  P(r)  ---->    U(b)  P(b)
       *           \                    \
       *           n(r)                  n(r)
       */

			gparent->color = RED;
			parent->color = BLACK;
			gparent->right->color = BLACK;

			if (!ggparent) { // gparent(G) is the root node
				/*
         * Case 2.4.1
         *
         *      G(b)                G(b)
         *      /  \       flip     /   \
         *    U(r)  P(r)  ---->    U(b)  P(b)
         *           \                    \
         *           n(r)                  n(r)
         */
				gparent->color = BLACK;
			} else if (!rb_is_red(ggparent)) {
				// ggparent is black, do nothing
				/*
         * Case 2.4.2
         *
         *        GGP(b)
         *        /    \
         *             G(r)
         *             /  \
         *           U(b)  P(b)
         *                  \
         *                  n(r)
         *
         * here GGP(grand grand parent)'s left substree must have a
         * black node
         */
			} else { // ggparent is red

				if (ggparent->left == gparent) {
					/*
           * Case 2.4.3.1
           *
           * here GGP(grand grand parent)'s right substree(α) must have
           * one and only one black node
           *
           *
           *    GGP(r)              G(r)                  G(b)
           *     /  \               /   \                 /   \
           *   G(r)  α   rotate   U(b) GGP(r)    flip   U(b) GGP(r)
           *   /  \      ----->         /  \    ----->        /  \
           * U(b)  P(b)               P(b)  α               P(b)  α
           *         \                 /                     /
           *         n(r)             n(r)                 n(r)
           *
           */
					struct rb_node *gggparent =
						rb_parent(ggparent);

					nr = rotate(ggparent, 1);
					nr->color = BLACK;

					if (!gggparent) {
						// ggparent is NULL means gparent is root
						root = nr;
					} else if (gggparent->left ==
						   ggparent) {
						gggparent->left = nr;
					} else if (gggparent->right ==
						   ggparent) {
						gggparent->right = nr;
					}

				} else {
					/*
           * Case 2.4.3.2
           *
           * here GGP(grand grand parent)'s right substree(α) must have
           * one and only one black node
           *
           *
           *   GGP(r)                  G(r)                  G(b)
           *   /    \                  /   \                 /   \
           *  α    G(r)     rotate   GGP(r) P(b)    flip   GGP(r) P(b)
           *       /  \     ----->    / \     \    ----->  / \      \
           *     U(b)  P(b)          α   U(b) n(r)        α  U(b)   n(r)
           *            \
           *            n(r)
           *
           */

					struct rb_node *gggparent =
						rb_parent(ggparent);

					nr = rotate(ggparent, 0);
					nr->color = BLACK;

					if (!gggparent) {
						// ggparent is NULL means gparent is root
						root = nr;
					} else if (gggparent->left ==
						   ggparent) {
						gggparent->left = nr;
					} else if (gggparent->right ==
						   ggparent) {
						gggparent->right = nr;
					}
				}
			}
		}
	}

	return root;
}

struct rb_node *find(struct rb_node *root, int val)
{
	if (!root)
		return NULL;
	else if (root->val > val)
		return find(root->left, val);
	else if (root->val < val)
		return find(root->right, val);
	else
		return root;
}

#if 0
struct RBTreeNode *remove(struct RBTreeNode *root, int val) {
  struct RBTreeNode *tmp;

  if (!root)
    ;
  else if (root->val > val) {
    root->left = remove(root->left, val);
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
      root->right = remove(root->right, val);
    } else {
      tmp = root->right;
      while (tmp->left)
        tmp = tmp->left;
      root->val = tmp->val;
      root->right = remove(root->right, root->val);
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

  /* If the removed is an orphan node */
  if (root)
    root->height = height(root);

  return root;
}
#endif
int maxdepth(struct rb_node *root)
{
	int d1, d2;

	if (!root)
		return -1;
	else {
		d1 = maxdepth(root->left) + 1;
		d2 = maxdepth(root->right) + 1;
		return d1 > d2 ? d1 : d2;
	}
}

void print(struct rb_node *root)
{
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
			if (count <= fullcount)
				printf(",nil");
		} else {
			if (count > 1)
				printf(",");

			printf("%d%c", node->val, node->color ? 'b' : 'r');

			enqueue(&queue, node->left);
			enqueue(&queue, node->right);
		}

		if (queueEmpty(&queue))
			printf("\n");
	}
}

int main()
{
	struct rb_node *root = NULL;
	root = insert(root, 5);
	// print(root);
	root = insert(root, 7);
	// print(root);
	root = insert(root, 3);
	// print(root);
	root = insert(root, 2);
	root = insert(root, 10);
	root = insert(root, 15);
	root = insert(root, 9);
	root = insert(root, 8);
	print(root);
	root = insert(root, 6);
	print(root);
	/*
  root = insert(root, 11);
  print(root);
  root = insert(root, 12);
  print(root);
  root = insert(root, 13);
  print(root);
  root = insert(root, 14);
  print(root);
  root = insert(root, 15);
  print(root);
  root = insert(root, 16);
  print(root);
  root = insert(root, 17);
  print(root);
  root = insert(root, 18);
  print(root);
  root = insert(root, 19);
  print(root);
  root = insert(root, 20);
  print(root);
  root = insert(root, 21);
  print(root);
  root = insert(root, 22);
  print(root);
  root = insert(root, 23);
  print(root);
  root = insert(root, 24);
  print(root);
  root = insert(root, 25);
  print(root);
  root = insert(root, 26);
  print(root);
  root = insert(root, 27);
  print(root);
  root = insert(root, 28);
  print(root);
  root = insert(root, 29);
  print(root);
  root = insert(root, 30);
  print(root);
   */

	/*
  printf("next of -1: %d\n", next(root, -1)->val);
  printf("next of 0: %d\n", next(root, 0)->val);
  printf("next of 2: %d\n", next(root, 2)->val);
  printf("next of 29: %d\n", next(root, 29)->val);
  printf("next of 6: %d\n", next(root, 6)->val);
  */
}
