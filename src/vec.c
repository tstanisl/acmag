#include "vec.h"
#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct vec {
	int used;
	int capacity;
	int item_size;
	char data[];
};

void *vec_create(int item_size, int min_capacity)
{
	int capacity = 8;
	while (capacity < min_capacity)
		capacity *= 2;

	struct vec *v = malloc(sizeof (*v) + item_size * capacity);
	if (!v)
		return NULL;

	v->used = 0;
	v->capacity = capacity;
	v->item_size = item_size;

	return v->data;
}

void vec_destroy(void *ptr)
{
	if (!ptr)
		return;
	struct vec *v = container_of(ptr, struct vec, data);
	free(v);
}

int vec_size(void *ptr)
{
	struct vec *v = container_of(ptr, struct vec, data);
	return v->used;
}

int vec_resize(void **ptr, int new_size)
{
	struct vec *v = container_of(*ptr, struct vec, data);
	if (new_size <= v->capacity) {
		v->used = new_size;
		return 1;
	}

	void *new_ptr = vec_create(v->item_size, new_size);
	if (!new_ptr)
		return 0;

	struct vec *new_v = container_of(new_ptr, struct vec, data);

	memcpy(new_v->data, v->data, v->item_size * v->used);
	free(v);
	new_v->used = new_size;
	*ptr = new_v->data;

	return 1;
}

void vec_test(void)
{
	int *tab = vec_create(sizeof tab[0], 0);

	for (int j = 0; j < 30; ++j) {
		printf("vsize=%d   tab={", vec_size(tab));
		for (int i = 0; i < vec_size(tab); ++i)
			printf(" %d", tab[i]);
		printf("}\n");
		VEC_PUSH(tab, j * j);
	}
	vec_destroy(tab);

	struct XY { int x, y; } xy = { 1, 2};
	struct XY *xytab = vec_create(sizeof xytab[0], 0);

	for (int j = 0; j < 30; ++j) {
		printf("vsize=%d   tab={", vec_size(xytab));
		for (int i = 0; i < vec_size(xytab); ++i)
			printf(" %p", (void*)&xytab[i]);
		printf("}\n");
		VEC_PUSH(xytab, xy);
	}
	vec_destroy(xytab);
}
