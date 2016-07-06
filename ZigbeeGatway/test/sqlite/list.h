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

/*��ͷ�ڵ�������һ���ڵ㣬����list��ͷ�ڵ�ָ�룬item��Ҫ��ӵĽڵ��ָ��*/
static  void dl_list_add ( struct dl_list *list, struct dl_list *item )
{
	item->next = list->next;
	item->prev = list;
	list->next->prev = item;
	list->next = item;
}

/*����Ϊһ��˫��ѭ������list�ǽڵ������ͷָ�룬
  ��ͷǰ����Ӽ��������ӣ�item����ӵĽڵ������*/
static  void dl_list_add_tail ( struct dl_list *list, struct dl_list *item )
{
	dl_list_add ( list->prev, item );
}

/*ɾ���ڵ㣬ɾ����ǰ�ڵ�*/
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

/*���������Ա�õ��ṹ���׵�ַ*/
#define dl_list_entry(item, type, member) \
	((type *) ((char *) item - offsetof(type, member)))




#define dl_list_first(list, type, member) \
	(dl_list_empty((list)) ? NULL : \
	 dl_list_entry((list)->next, type, member))

#define dl_list_last(list, type, member) \
	(dl_list_empty((list)) ? NULL : \
	 dl_list_entry((list)->prev, type, member))




/*�����ṹ�壬item����ʱ����Ľṹ�壬list��ͷ�ڵ㣬type�ǽṹ�����ͣ�member�ǽṹ���Ա
  ���Ȼ�õڶ����ṹ���ַ��Ȼ���жϵ�ǰ�ṹ��Ľڵ��Ƿ�Ϊͷ�ڵ㣬Ȼ���ȡ��һ���ṹ���ַ
  �˱������Թ���һ���ṹ�壬������Ҫ�Լ��ֶ�ȥ������һ���ṹ���е�����*/
#define dl_list_for_each(item, list, type, member) \
	for (item = dl_list_entry((list)->next, type, member); \
			&item->member != (list); \
			item = dl_list_entry(item->member.next, type, member))

/*�����ṹ�壬ͬ�ϣ�n����type���͵Ľṹ���һ�����������ȵõ��ڶ����ṹ��͵������ṹ��
  Ȼ�󽫵��������ڶ���������ȡ�����ڵ㣬ÿ��ͬʱ��ȡ�����ṹ�壬��2��3��3��4��*/
#define dl_list_for_each_safe(item, n, list, type, member) \
	for (item = dl_list_entry((list)->next, type, member), \
			n = dl_list_entry(item->member.next, type, member); \
			&item->member != (list); \
			item = n, n = dl_list_entry(n->member.next, type, member))

/*�����ṹ�壬ͬ������ı��������Ȼ�����һ���ṹ���ַ����һ���ṹ������������һ��*/
#define dl_list_for_each_reverse(item, list, type, member) \
	for (item = dl_list_entry((list)->prev, type, member); \
			&item->member != (list); \
			item = dl_list_entry(item->member.prev, type, member))

#define DEFINE_DL_LIST(name) \
	struct dl_list name = { &(name), &(name) }

#endif /* LIST_H */
