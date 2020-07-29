test: rbtree-test

rbtree-test:
	clang -g tests/rbtree.c queue.c rbtree.c -I. -o /tmp/rbtree-test
	/tmp/rbtree-test
