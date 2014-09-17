#ifndef VEC_H
#define VEC_H

void *vec_create(int item_size, int min_capacity);
void vec_destroy(void *vec);
int vec_size(void *vec);
int vec_resize(void **pvec, int new_size);

#define VEC_PUSH(vec,item) \
	(vec_resize((void**)&vec, vec_size(vec) + 1) ? \
	vec[vec_size(vec) - 1] = item, 1 : 0)

#define VEC_INIT(vec) \
	do { vec = vec_create(sizeof (vec)[0], 0); } while (0)

#endif /* VEC_H */
