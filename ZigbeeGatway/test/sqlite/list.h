/*
 * Doubly-linked list
 * Copyright (c) 2009, Jouni Malinen <j@w1.fi>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Alternatively, this software may be distributed under the terms of BSD
 * license.
 *
 * See README and COPYING for more details.
 */

#ifndef LIST_H
#define LIST_H

#ifndef NULL
#define NULL  ((void *)0)
#endif

/**
 * struct dl_list - Doubly-linked list
 */
struct dl_list
{
	struct dl_list *next;
	struct dl_list *prev;
};

static  void dl_list_init ( struct dl_list *list )
{
	list->next = list;
	list->prev = list;
}

/*在头节点后面添加一个节点，参数list是头节点指针，item是要添加的节点的指针*/
static  void dl_list_add ( struct dl_list *list, struct dl_list *item )
{
	item->next = list->next;
	item->prev = list;
	list->next->prev = item;
	list->next = item;
}

/*链表为一个双向循环链表，list是节点的链表头指针，
  在头前面添加即在最后添加，item是添加的节点的链表*/
static  void dl_list_add_tail ( struct dl_list *list, struct dl_list *item )
{
	dl_list_add ( list->prev, item );
}

/*删除节点，删除当前节点*/
static  void dl_list_del ( struct dl_list *item )
{
	item->next->prev = item->prev;
	item->prev->next = item->next;
	item->next = NULL;
	item->prev = NULL;
}

static  int dl_list_empty ( struct dl_list *list )
{
	return list->next == list;
}

static  unsigned int dl_list_len ( struct dl_list *list )
{
	struct dl_list *item;
	int count = 0;
	for ( item = list->next; item != list; item = item->next )
	{
		count++;
	}
	return count;
}

#ifndef offsetof
#define offsetof(type, member) ((long) &((type *) 0)->member)
#endif

/*根据链表成员得到结构体首地址*/
#define dl_list_entry(item, type, member) \
	((type *) ((char *) item - offsetof(type, member)))




#define dl_list_first(list, type, member) \
	(dl_list_empty((list)) ? NULL : \
	 dl_list_entry((list)->next, type, member))

#define dl_list_last(list, type, member) \
	(dl_list_empty((list)) ? NULL : \
	 dl_list_entry((list)->prev, type, member))




/*遍历结构体，item是临时定义的结构体，list是头节点，type是结构体类型，member是结构体成员
  首先获得第二个结构体地址，然后判断当前结构体的节点是否为头节点，然后获取下一个结构体地址
  此遍历会略过第一个结构体，所以需要自己手动去操作第一个结构体中的数据*/
#define dl_list_for_each(item, list, type, member) \
	for (item = dl_list_entry((list)->next, type, member); \
			&item->member != (list); \
			item = dl_list_entry(item->member.next, type, member))

/*遍历结构体，同上，n代表type类型的结构体的一个变量，首先得到第二个结构体和第三个结构体
  然后将第三个给第二个，向后获取其他节点，每次同时获取两个结构体，即2、3，3、4等*/
#define dl_list_for_each_safe(item, n, list, type, member) \
	for (item = dl_list_entry((list)->next, type, member), \
			n = dl_list_entry(item->member.next, type, member); \
			&item->member != (list); \
			item = n, n = dl_list_entry(n->member.next, type, member))

/*遍历结构体，同上上面的遍历，首先获得最后一个结构体地址，第一个结构体是链表的最后一个*/
#define dl_list_for_each_reverse(item, list, type, member) \
	for (item = dl_list_entry((list)->prev, type, member); \
			&item->member != (list); \
			item = dl_list_entry(item->member.prev, type, member))

#define DEFINE_DL_LIST(name) \
	struct dl_list name = { &(name), &(name) }

#endif /* LIST_H */
