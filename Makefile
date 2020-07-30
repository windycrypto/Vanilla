test: rbtree-test avltree-test avltree2-test

rbtree-test:
	clang tests/rbtree.c queue.c rbtree.c -I. -o /tmp/rbtree-test
	/tmp/rbtree-test

avltree-test:
	clang tests/avltree.c queue.c avltree.c -I. -o /tmp/avltree-test
	/tmp/avltree-test

avltree2-test:
	clang tests/avltree2.c queue.c avltree2.c -I. -o /tmp/avltree2-test
	/tmp/avltree2-test
