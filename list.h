#ifndef LIST_H
#define LIST_H __FILE__

#include "common.h"

struct list {
	struct list *next, *prev;
};

inline void list_add(struct list *head, struct list *node)
{
	node->next = head->next;
	node->prev = head;
	head->next->prev = node;
	head->next = node;
}

inline void list_del(struct list *node)
{
	node->next->prev = node->prev;
	node->prev->next = node->next;
}

#endif /* LIST_H */
