test: rbtree-test avltree-test

rbtree-test:
	clang -g tests/rbtree.c queue.c rbtree.c -I. -o /tmp/rbtree-test
	/tmp/rbtree-test

avltree-test:
	clang -g tests/avltree.c queue.c avltree.c -I. -o /tmp/avltree-test
	/tmp/avltree-test
