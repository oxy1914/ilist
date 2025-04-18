/* SPDX-License-Identifier: GPL-2.0 OR Apache-2.0 WITH LLVM-exception */
/* Copyright (C) 2025 - Aleksandr Loktionov */
#include <acutest.h>

#define ILIST_IMPLEMENTATION
#define ILIST_IDX_T short int
#include <ilist.h>

typedef struct {
	int data;
	ilist_head list;
} my_struct;

void tst_list_empty(void)
{
	ilist_head my_list;
	my_struct itemX;

	ILIST_INIT(&my_list);

	TEST_CHECK_(sizeof(ILIST_IDX_T) >= sizeof(short int), "%lu >= %lu", sizeof(ILIST_IDX_T), sizeof(short int));
	TEST_CHECK(ilist_empty(&my_list));
	TEST_CHECK(!ilist_is_singular(&my_list));
	TEST_CHECK(!ilist_is_first(&itemX.list, &my_list));
	TEST_CHECK(!ilist_is_last(&itemX.list, &my_list));
}

void tst_list_singular(void)
{
	ilist_head my_list;
	my_struct itemX = {.data = 42};
	ILIST_INIT(&my_list);

	ilist_add(&itemX.list, &my_list);

	TEST_CHECK(!ilist_empty(&my_list));
	TEST_CHECK(ilist_is_singular(&my_list));
	TEST_CHECK(ilist_is_first(&itemX.list, &my_list));
	TEST_CHECK(ilist_is_last(&itemX.list, &my_list));
	TEST_CHECK(ilist_next_entry(&itemX, list) != &itemX);
}

void tst_list_dual(void)
{
	ilist_head my_list;
	my_struct item1 = {.data = 1};
	my_struct item2 = {.data = 2};
	ILIST_INIT(&my_list);
	
	ilist_add_tail(&item1.list, &my_list);
	ilist_add_tail(&item2.list, &my_list);

	TEST_CHECK(!ilist_empty(&my_list));
	TEST_CHECK(!ilist_is_singular(&my_list));
	TEST_CHECK(ilist_is_first(&item1.list, &my_list));
	TEST_CHECK(ilist_is_last(&item2.list, &my_list));
	TEST_CHECK(&item2 == ilist_next_entry(&item1, list));
	TEST_CHECK(&item1 == ilist_prev_entry(&item2, list));
}

void tst_list_complex(void)
{
	ilist_head my_list;
	my_struct item0 = {.data = 0};
	my_struct item1 = {.data = 1};
	my_struct item2 = {.data = 2};
	my_struct item3 = {.data = 3};
	ILIST_INIT(&my_list);

	ilist_add(&item1.list, &my_list);
	ilist_add(&item0.list, &my_list);
	ilist_add_tail(&item3.list, &my_list);
	ilist_add_before_tail(&item2.list, &my_list);

	/*assert list state*/
	TEST_CHECK(!ilist_empty(&my_list));
	TEST_CHECK(!ilist_is_singular(&my_list));
	/*assert first*/
	TEST_CHECK(&item0 == ilist_first_entry(&my_list, my_struct, list));
	TEST_CHECK(ilist_is_first(&item0.list, &my_list));
	TEST_CHECK(!ilist_is_last(&item0.list, &my_list));
	/*assert last*/
	TEST_CHECK(&item3 == ilist_last_entry(&my_list, my_struct, list));
	TEST_CHECK(!ilist_is_first(&item3.list, &my_list));
	TEST_CHECK(ilist_is_last(&item3.list, &my_list));
	/*asssert middle*/
	TEST_CHECK(!ilist_is_first(&item2.list, &my_list));
	TEST_CHECK(!ilist_is_last(&item2.list, &my_list));
	TEST_CHECK(!ilist_is_first(&item1.list, &my_list));
	TEST_CHECK(!ilist_is_last(&item1.list, &my_list));
	/*assert sequence*/
	TEST_CHECK(&item1 == ilist_next_entry(&item0, list));
	TEST_CHECK(&item2 == ilist_next_entry(&item1, list));
	TEST_CHECK(&item3 == ilist_next_entry(&item2, list));
	/*assert sequence reverse*/
	TEST_CHECK(&item2 == ilist_prev_entry(&item3, list));
	TEST_CHECK(&item1 == ilist_prev_entry(&item2, list));
	TEST_CHECK(&item0 == ilist_prev_entry(&item1, list));
}

void tst_list_foreach(void)
{
	const int seq_expected[] = {7, 6, 5, 4, 0, 1, 2, 3};
	ilist_head my_list;
	my_struct items[8] = {0};
	ILIST_INIT(&my_list);
	for (int i = 0; i < 8; ++i)
		items[i].data = i;
	ilist_add_tail(&items[0].list, &my_list);
	ilist_add_tail(&items[1].list, &my_list);
	ilist_add_tail(&items[2].list, &my_list);
	ilist_add_tail(&items[3].list, &my_list);
	ilist_add(&items[4].list, &my_list);
	ilist_add(&items[5].list, &my_list);
	ilist_add(&items[6].list, &my_list);
	ilist_add(&items[7].list, &my_list);

	/*assert expected sequence*/
	{
		int i = 0;
		my_struct *item;

		ilist_for_each_entry(item, &my_list, list) {
			TEST_CHECK_(seq_expected[i] == item->data, "seq_expected[%d] == item->data;//%d == %d", i, seq_expected[i], item->data);
			++i;
		}
	}
	TEST_CHECK(!ilist_empty(&my_list));
	TEST_CHECK(!ilist_is_singular(&my_list));

	/*
	 *delete fist&last entries, leave only two middle {4,0}
	 */
	for (int i = 0; i < 3; ++i) {
		ilist_del(&ilist_first_entry(&my_list, my_struct, list)->list);
		ilist_del(&ilist_last_entry(&my_list, my_struct, list)->list);
	}

	TEST_CHECK(!ilist_empty(&my_list));
	TEST_CHECK(!ilist_is_singular(&my_list));
	/*assert first*/
	TEST_CHECK(&items[4] == ilist_first_entry(&my_list, my_struct, list));
	TEST_CHECK(ilist_is_first(&items[4].list, &my_list));
	TEST_CHECK(!ilist_is_last(&items[4].list, &my_list));
	/*assert last*/
	TEST_CHECK(&items[0] == ilist_last_entry(&my_list, my_struct, list));
	TEST_CHECK(!ilist_is_first(&items[0].list, &my_list));
	TEST_CHECK(ilist_is_last(&items[0].list, &my_list));
	/*assert sequence*/
	TEST_CHECK(&items[0] == ilist_next_entry(&items[4], list));
	TEST_CHECK(&items[4] == ilist_prev_entry(&items[0], list));

	/*
         *delete first, make singular
	 */
	ilist_del(&items[4].list);

	TEST_CHECK(!ilist_empty(&my_list));
	TEST_CHECK(ilist_is_singular(&my_list));
	TEST_CHECK(ilist_is_first(&items[0].list, &my_list));
	TEST_CHECK(ilist_is_last(&items[0].list, &my_list));
	TEST_CHECK(!ilist_is_first(&items[4].list, &my_list));
	TEST_CHECK(!ilist_is_last(&items[4].list, &my_list));

	/*
         * delete last, make empty
         */
	ilist_del(&items[0].list);
	TEST_CHECK(ilist_empty(&my_list));
	TEST_CHECK(!ilist_is_singular(&my_list));
	TEST_CHECK(!ilist_is_first(&items[0].list, &my_list));
	TEST_CHECK(!ilist_is_last(&items[0].list, &my_list));
	TEST_CHECK(!ilist_is_first(&items[4].list, &my_list));
	TEST_CHECK(!ilist_is_last(&items[4].list, &my_list));
}

TEST_LIST = {
	{"list empty",		tst_list_empty},
	{"list singular",	tst_list_singular},
	{"list dual",		tst_list_dual},
	{"list complex",	tst_list_complex},
	{"list foreach",	tst_list_foreach},
	{NULL, NULL}
};
