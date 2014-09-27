#ifndef REF_H
#define REF_H

struct ref;
typedef void (*ref_dtor_cb)(struct ref *);

struct ref {
	ref_dtor_cb dtor;
	int cnt;
};

static inline void ref_init(struct ref *ref, ref_dtor_cb dtor)
{
	ref->cnt = 1;
	ref->dtor = dtor;
}

static inline struct ref *ref_get(struct ref *ref)
{
	++ref->cnt;
	return ref;
}

static inline void ref_put(struct ref *ref)
{
	if (--ref->cnt == 0)
		ref->dtor(ref);
}

#endif
