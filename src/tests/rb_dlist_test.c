#include <stdlib.h>
#include <unistd.h>
#include <malloc.h>
#include <string.h>
#include <stdio.h>

#include "rb_dlist.h"

#define DLIST_MAX_LEN	5

int main(int argc, char **argv)
{
	int i = 0;
	dlist list;

	memset(&list, 0, sizeof(dlist));

	rb_dlist_init(&list);

	for(i = 0; i < DLIST_MAX_LEN; i++) {
		dlist_elem * elem = NULL;
		elem = (dlist_elem *)malloc(sizeof(dlist_elem));
		if(elem == NULL)
			continue;

		rb_dlist_push_front(&list, elem);
	}

	printf("double list size: %d\n", rb_dlist_size(&list));

	for(i = 0; i < DLIST_MAX_LEN; i++) {
		dlist_elem * elem = rb_dlist_pop_back(&list);
		if(elem != NULL) {
			free(elem);
			elem = NULL;
		}
	}

	printf("list if empty: %s\n", rb_dlist_empty(&list) ? "empty":"no empty");

	return 0;
}
