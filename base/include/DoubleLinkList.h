#ifndef __DOUBLELINKLIST_H__
#define __DOUBLELINKLIST_H__

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>



namespace Base {

#define container_of(ptr, type, member) ({ \
    const typeof( ((type *)0)->member ) *__mptr = (ptr); \
    (type *)((char *)__mptr - offsetof(type,member));})

typedef struct dlist_elem_st DLListElem;

/* Double list element. */
typedef struct dlist_elem_st {
	DLListElem *prev; /* Previous list element. */
    DLListElem *next; /* Next list element. */
} DLListElem;

/* Double list. */
typedef struct dlist_st {
	DLListElem head; /* List head. */
	DLListElem tail; /* List tail. */
} DLList;

class CDoubleLinkList
{
public:
	CDoubleLinkList();
	~CDoubleLinkList();

	void PushFront(DLListElem *elem);
	void PushBack(DLListElem *elem);

	DLListElem *PopFront();
	DLListElem *PopBack();

	bool Empty();
	size_t Size();

private:
	DLList m_dllist;
};

} // end namespace

#endif


