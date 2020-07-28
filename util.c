#include <stdio.h>
#include <stdlib.h>

#include "include/queue.h"
#include "include/rbtree.h"

int rbtree_depth(struct rb_node *root)
{
	int d1, d2;

	if (!root)
		return -1;
	else {
		d1 = rbtree_depth(root->left) + 1;
		d2 = rbtree_depth(root->right) + 1;
		return d1 > d2 ? d1 : d2;
	}
}

/*
 * print a tree in leetcode format, this method only applies for printing a balanced tree.
 * Printing unbalanced tree may end up with printing excessive "nil"s
 */
void rbtree_dump(struct rb_node *root)
{
	int mdepth, fullcount, count = 0;

	queue_t *queue = create_queue(free);

	mdepth = rbtree_depth(root);
	fullcount = (2 << mdepth) - 1;

	enqueue(queue, root);
	while (queue->size) {
		root = (struct rb_node *)dequeue(queue);

		count++;

		if (!root) {
			if (count <= fullcount)
				printf(",nil");
		} else {
			if (count > 1)
				printf(",");

			printf("%d%c", root->val, root->color ? 'b' : 'r');

			enqueue(queue, root->left);
			enqueue(queue, root->right);
		}

		if (queue->size < 1)
			printf("\n");
	}
}
