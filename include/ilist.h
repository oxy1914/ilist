/* SPDX-License-Identifier: GPL-2.0 OR Apache-2.0 WITH LLVM-exception */
/* Copyright (C) 2025 - Aleksandr Loktionov */
#ifndef _ILIST_H_
#define _ILIST_H_

#ifndef ILIST_IDX_T
#define ILIST_IDX_T short signed int
#endif

#ifndef ILDEF
#define ILDEF static inline
#endif

typedef struct {
	ILIST_IDX_T next, prev;
} __attribute__((aligned(2 * sizeof(ILIST_IDX_T)))) ilist_head;

#define ILIST_INIT(P)                      \
	do {                               \
		(P)->next = (P)->prev = 0; \
	} while (0)

#define ilist_is_first(P, H) ((P) == &(H)[(H)->next])

#define ilist_is_last(P, H) ((P) == &(H)[(H)->prev])

#define ilist_entry(P, T, M) ((T *)((char *)(P) - __builtin_offsetof(T, M)))

#define ilist_first_entry(P, T, M) ilist_entry(&(P)[(P)->next], T, M)

#define ilist_last_entry(P, T, M) ilist_entry(&(P)[(P)->prev], T, M)

#define ilist_next_entry(P, M) \
	ilist_entry(&(P)->M + (P)->M.next, __typeof__(*P), M)

#define ilist_prev_entry(P, M) \
	ilist_entry(&(P)->M + (P)->M.prev, __typeof__(*P), M)

#define ilist_for_each_entry(P, H, M)                                   \
	for (P = ilist_first_entry(H, __typeof__(*P), M); &P->M != (H); \
	     P = ilist_next_entry(P, M))

#define ilist_empty(H) ((H)->next == 0 && (H)->prev == 0)
#define ilist_is_singular(H) ((H)->next == (H)->prev && (H)->next != 0)

ILDEF void ilist_add_(ilist_head *new, ilist_head *prev,
			      ilist_head *next);
ILDEF void ilist_add(ilist_head *new, ilist_head *head);
ILDEF void ilist_add_tail(ilist_head *new, ilist_head *head);
ILDEF void ilist_add_before_tail(ilist_head *new, ilist_head *head);
ILDEF void ilist_del_(ilist_head *prev, ilist_head *next);
ILDEF void ilist_del(ilist_head *old);

#ifdef ILIST_IMPLEMENTATION
ILDEF void ilist_add_(ilist_head *new, ilist_head *prev,
			      ilist_head *next)
{
	next->prev = new - next;
	new->next = next - new;
	new->prev = prev - new;
	prev->next = new - prev;
}
ILDEF void ilist_add(ilist_head *new, ilist_head *head)
{
	ilist_add_(new, head, &head[head->next]);
}
ILDEF void ilist_add_tail(ilist_head *new, ilist_head *head)
{
	ilist_add_(new, &head[head->prev], head);
}
ILDEF void ilist_add_before_tail(ilist_head *new, ilist_head *head)
{
	head = head + head->prev;
	ilist_add_(new, &head[head->prev], head);
}

ILDEF void ilist_del_(ilist_head *prev, ilist_head *next)
{
	next->prev = prev - next;
	prev->next = next - prev;
}
ILDEF void ilist_del(ilist_head *old)
{
	ilist_del_(&old[old->prev], &old[old->next]);
#ifdef DEBUG
	old->next = old->prev = 0;
#endif
}
#undef ILIST_IMPLEMENTATION
#endif /* ILIST_IMPLEMENTATION */
#endif /* _ILIST_H_ */
