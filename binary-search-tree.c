#include <stdio.h>
#include <stdlib.h>

typedef struct tree_node_ {
	void *key;
	void *val;
	struct tree_node_ *left;
	struct tree_node_ *right;
} tree_node_t;

typedef struct tree_ {
	tree_node_t *root;
	// return 0 if two nodes equal, >0 if target greater than the traversed one,
	// <0 for vice
	int (*compare_fn)(void *current, void *target);
} tree_t;

tree_t *create_tree(int (*compare_fn)(void *, void *))
{
	tree_t *t;

	t = calloc(1, sizeof *t);
	if (!t) {
		fprintf(stderr,
			"error allocating memory for the creating queue\n");
		abort();
	}
	t->compare_fn = compare_fn;

	return t;
}

// this implementation ignores the duplicate nodes, if we are inserting a
// node which is already existing in the tree, nothing will happen
tree_node_t *_insert(tree_t *t, tree_node_t *root, void *key)
{
	if (root) {
		int rv = t->compare_fn(root->key, key);
		if (rv < 0) {
			root->left = _insert(t, root->left, key);
		} else if (rv > 0) {
			root->right = _insert(t, root->right, key);
		} // do nothing if they are equal
	} else {
		root = calloc(1, sizeof *root);
		root->key = key;
	}
	return root;
}

tree_t *insert(tree_t *t, void *key)
{
	t->root = _insert(t, t->root, key);
	return t;
}

tree_node_t *_find(tree_t *t, tree_node_t *root, void *key)
{
	if (!root)
		return NULL;

	int rv = t->compare_fn(root->key, key);
	if (rv < 0) {
		return _find(t, root->left, key);
	} else if (rv > 0) {
		return _find(t, root->right, key);
	} else {
		return root;
	}
}

tree_node_t *find(tree_t *t, void *key)
{
	return _find(t, t->root, key);
}

tree_node_t *_delete(tree_t *t, tree_node_t *root, void *key)
{
	if (!root)
		return root;

	tree_node_t *tmp;
	int rv = t->compare_fn(root->key, key);
	if (rv < 0)
		root->left = _delete(t, root->left, key);
	else if (rv > 0)
		root->right = _delete(t, root->right, key);
	else if (root->left && root->right) {
		tmp = root->right;
		while (tmp->left)
			tmp = tmp->left;
		root->key = tmp->key;
		root->val = tmp->val;
		root->right = _delete(t, root->right, root->key);
	} else { // one or zero children
		tmp = root;
		if (!root->left) // also handles 0 children
			root = root->right;
		else if (!root->right)
			root = root->left;
		free(tmp);
	}

	return root;
}

void delete (tree_t *t, void *key)
{
	t->root = _delete(t, t->root, key);
}

int main()
{
}
