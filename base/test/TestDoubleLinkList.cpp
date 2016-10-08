#include <stdlib.h>
#include <unistd.h>
//#include <malloc.h>
#include <string.h>
#include <stdio.h>

#include "DoubleLinkList.h"



#define DLIST_MAX_LEN	5

typedef struct dlist_test_st {
	int count;
	Base::DLListElem elem;
}dlist_test;

typedef enum push_method_en {
	PUSH_FRONT,
	PUSH_BACK,
}push_method;

typedef enum pop_method_en {
	POP_FRONT,
	POP_BACK,
}pop_method;

void dlist_push_pop_test(push_method push, pop_method pop)
{
	int i = 0;
	Base::CDoubleLinkList dllist;
	
	printf("push: %d, pop: %d, %s\n", push, pop, dllist.Empty() ? "empty":"no empty");

	for(i = 0; i < DLIST_MAX_LEN; i++) {
		dlist_test * ltest = NULL;
		ltest = (dlist_test *)malloc(sizeof(dlist_test));
		if(ltest == NULL) {
			continue;
		}

		memset(ltest, 0, sizeof(dlist_test));
		ltest->count = i;

		if(push == PUSH_FRONT) {
			dllist.PushFront(&ltest->elem);
		} else {
			dllist.PushBack(&ltest->elem);
		}
	}

	printf("double list size: %lu\n", dllist.Size());

	for(i = 0; i < DLIST_MAX_LEN; i++) {
		Base::DLListElem * elem = NULL;
		
		if(pop == POP_FRONT) {
			elem = dllist.PopFront();
		} else {
			elem = dllist.PopBack();
		}

		if(elem == NULL) {
			continue;
		}

		dlist_test * ltest = container_of(elem, dlist_test, elem);
		printf("pop count: %d\n", ltest->count);
		free(ltest);
		ltest = NULL;
	}

	printf("list if empty: %s\n\n", dllist.Empty() ? "empty":"no empty");

	return;
}

int main(int argc, char **argv)
{
	dlist_push_pop_test(PUSH_FRONT, POP_FRONT);
	dlist_push_pop_test(PUSH_FRONT, POP_BACK);

	dlist_push_pop_test(PUSH_BACK, POP_FRONT);
	dlist_push_pop_test(PUSH_BACK, POP_BACK);

	return 0;
}


