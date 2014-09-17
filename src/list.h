#ifndef LIST_H
#define LIST_H __FILE__

#include "common.h"

struct list {
	struct list *next, *prev;
};

static inline void list_add(struct list *node, struct list *head)
{
	node->next = head->next;
	node->prev = head;
	head->next->prev = node;
	head->next = node;
}

static inline void list_add_tail(struct list *node, struct list *head)
{
	node->prev = head->prev;
	node->next = head;
	head->prev->next = node;
	head->prev = node;
}

static inline void list_del(struct list *node)
{
	node->next->prev = node->prev;
	node->prev->next = node->next;
}

#define list_entry(ptr,type,member) \
	container_of(ptr,type,member)

static inline void list_init(struct list *node)
{
	node->next = node;
	node->prev = node;
}

#define DEFINE_LIST(name) struct list name = { .next = &name, .prev = &name }

#define list_foreach(it,head) \
	for (struct list *it = (head)->next, *__next; \
	     __next = (it)->next, it != (head); it = __next)

#endif /* LIST_H */
