#include "base/DoubleLinkList.h"



namespace Base {

static inline char elem_is_head(DLListElem *elem)
{
	return elem != NULL && elem->prev == NULL && elem->next != NULL;
}

static inline char elem_is_interior(DLListElem *elem)
{
	return elem != NULL && elem->prev != NULL && elem->next != NULL;
}

static inline char elem_is_tail(DLListElem *elem)
{
	return elem != NULL && elem->prev != NULL && elem->next == NULL;
}

static inline DLListElem * rb_dlist_begin(DLList *list)
{
	if(list == NULL)
		return NULL;

  	return list->head.next;
}

static inline DLListElem *rb_dlist_next(DLListElem *elem)
{
	if(elem_is_head(elem) || elem_is_interior(elem))
  		return elem->next;
	else
		return NULL;
}

static inline DLListElem * rb_dlist_end(DLList *list)
{
	if(list == NULL)
		return NULL;

  	return &list->tail;
}

static inline DLListElem *rb_dlist_remove(DLListElem *elem)
{
	if(!elem_is_interior(elem))
		return NULL;

	elem->prev->next = elem->next;
	elem->next->prev = elem->prev;

	return elem->next;
}

static inline DLListElem *dlist_front(DLList *list)
{
	if(rb_dlist_begin(list) == rb_dlist_end(list))
		return NULL;

	return list->head.next;
}

static inline DLListElem *dlist_back(DLList *list)
{
	if(rb_dlist_begin(list) == rb_dlist_end(list))
		return NULL;

	return list->tail.prev;
}

static inline void dlist_insert(DLListElem *before, DLListElem *elem)
{
	if(before == NULL || elem == NULL)
		return;
  
	if(elem_is_interior(before) || elem_is_tail(before)) {
		elem->prev = before->prev;
		elem->next = before;
		before->prev->next = elem;
		before->prev = elem;
	}

	return;
}

/***********************************************************************************************************/

CDoubleLinkList::CDoubleLinkList()
{
	m_dllist.head.prev = NULL;
  	m_dllist.head.next = &m_dllist.tail;
  	m_dllist.tail.prev = &m_dllist.head;
  	m_dllist.tail.next = NULL;
}

CDoubleLinkList::~CDoubleLinkList()
{
}

void CDoubleLinkList::PushFront(DLListElem *elem)
{
	dlist_insert(rb_dlist_begin(&m_dllist), elem);
}

void CDoubleLinkList::PushBack(DLListElem *elem)
{
	dlist_insert(rb_dlist_end(&m_dllist), elem);
}

DLListElem *CDoubleLinkList::PopFront()
{
	DLListElem *front = dlist_front(&m_dllist);

	rb_dlist_remove(front);

	return front;
}

DLListElem *CDoubleLinkList::PopBack()
{
	DLListElem *back = dlist_back(&m_dllist);

	rb_dlist_remove(back);

	return back;
}

bool CDoubleLinkList::Empty()
{
	return rb_dlist_begin(&m_dllist) == rb_dlist_end(&m_dllist);
}

size_t CDoubleLinkList::Size()
{
	DLListElem *e = NULL;
	size_t cnt = 0;

	for (e = rb_dlist_begin(&m_dllist); e != rb_dlist_end(&m_dllist); 
			e = rb_dlist_next(e)) {
		cnt++;
	}		

	return cnt;
}

} // end namespace


