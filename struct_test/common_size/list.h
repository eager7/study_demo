/*********************************************************************************
*Copyright(C),2010-2020, ����TCL����������޹�˾
*FileName:  List.h
*Author:	Li Peng
*Version:	V0.1
*Date:  //�������
*Description: ����������Ķ����Լ���Ӧ�Ĳ���
*History:  
1.Date:		2015.03.23
Author:		Li Peng
Modification:	��ʼ�汾
**********************************************************************************/

#ifndef _LIST_H_  
#define _LIST_H_  

/* BEGIN: BUGXXXX,modified by zhangheng for XXXREASONXXX on 2015-4-21 */
#ifndef NULL
#define NULL  ((void *)0)
#endif
/* END: BUGXXXX,modified by zhangheng on 2015-4-21 */

/**********************************************************
����: ����MEMBER��Ա��TYPE�ṹ���е�ƫ����
**********************************************************/
#define offsetof(TYPE, MEMBER)  (unsigned long)(&(((TYPE*)0)->MEMBER))  


/**********************************************************
����: ��������Ԫ�ص���ʼ��ַ
����:
ptr��   type�ṹ���е�����ָ��
type:   �ṹ������
member: �����Ա����
**********************************************************/
#define container_of(ptr, type, member) (type *)((char*)(ptr) - offsetof(type, member))  

#define LIST_HEAD_INIT(name) {&(name), &(name)}  

struct list
{
	struct list *prev, *next;
};

//dl_list_init
static inline void list_init(struct list *list)
{
	list->next = list;
	list->prev = list;
}

//dl_list_empty
static inline int list_empty(struct list *list)
{
	return list->next == list;
}

//dl_list_add_tail
// ��new_link���뵽link֮ǰ  
static inline void list_insert(struct list *link, struct list *new_link)
{
	new_link->prev = link->prev;
	new_link->next = link;
	new_link->prev->next = new_link;
	new_link->next->prev = new_link;
}

/**********************************************************
����: ��new_link�ڵ㸽�ӵ�list������
**********************************************************/
static inline void list_append(struct list *list, struct list *new_link)
{
	list_insert(list, new_link);
}

//dl_list_del  
/**********************************************************
����: ���������Ƴ��ڵ�
**********************************************************/
static inline void list_remove(struct list *link)
{
	link->prev->next = link->next;
	link->next->prev = link->prev;
}

/**********************************************************
��ȡlink�ڵ��Ӧ�Ľṹ�������ַ
link:   ����ڵ�ָ��
type:   �ṹ��������
member: �ṹ���Ա������
**********************************************************/
#define list_entry(link, type, member)  container_of(link, type, member)  

/**********************************************************
��ȡ����ͷ�ڵ��Ӧ�Ľṹ�������ַ
list:   ����ͷָ��
type:   �ṹ��������
member: �ṹ���Ա������
Note:
����ͷ�ڵ�ʵ��Ϊ����ͷ����һ���ڵ�,����ͷδʹ�ã��൱���ڱ�
**********************************************************/
#define list_head(list, type, member) list_entry((list)->next, type, member)  

/**********************************************************
��ȡ����β�ڵ��Ӧ�Ľṹ�������ַ
list:   ����ͷָ��
type:   �ṹ��������
member: �ṹ���Ա������
**********************************************************/
#define list_tail(list, type, member) list_entry((list)->prev, type, member)  

/**********************************************************
����������һ���ڵ��Ӧ�Ľṹ��ָ��
elm:    �ṹ�����ָ��
type:   �ṹ��������
member: �ṹ���Ա������(���������)
**********************************************************/
#define list_next(elm,type,member) list_entry((elm)->member.next, type, member)  

/**********************************************************
�����������нڵ��Ӧ�Ľṹ��
pos : �ṹ��ָ��
type : �ṹ��������
list : ����ͷָ��
member : �ṹ���Ա������(���������)
Note : ����ͷδʹ�ã���˱���������posָ��Ĳ�����Ч�Ľṹ���ַ
**********************************************************/
//#define list_for_each_entry(pos, type, list, member)    \  
/*for (pos = list_head(list, type, member);               \
	&pos->member != (list);                              \
	pos = list_next(pos, type, member))*/


/* BEGIN: BUGXXXX,modified by zhangheng for XXXREASONXXX on 2015-4-21 */
/* ���º����ֲ��zigbee��Ŀ�������ݿ�ԭ����Ҫ��ֲ */

static inline unsigned int dl_list_len ( struct list *list )
{
	struct list *item;
	int count = 0;
	for ( item = list->next; item != list; item = item->next )
	{
		count++;
	}
	return count;
}

#define dl_list_entry(item, type, member)   ((type *) ((char *) item - offsetof(type, member)))

#define dl_list_first(list, type, member) \
	(list_empty((list)) ? NULL : \
	 dl_list_entry((list)->next, type, member))

#define dl_list_last(list, type, member) \
	(list_empty((list)) ? NULL : \
	 dl_list_entry((list)->prev, type, member))


#define dl_list_for_each(item, list, type, member)\
    for(item = dl_list_entry((list)->next, type, member);\
        &item->member != (list);\
        item = dl_list_entry(item->member.next, type, member))

#define dl_list_for_each_safe(item, n, list, type, member)\
    for (item = dl_list_entry((list)->next, type, member), n = dl_list_entry(item->member.next, type, member);\
         &item->member != (list);\
         item = n, n = dl_list_entry(n->member.next, type, member))

#define dl_list_for_each_reverse(item, list, type, member)\
    for (item = dl_list_entry((list)->prev, type, member);\
         &item->member != (list);\
         item = dl_list_entry(item->member.prev, type, member))

#define DEFINE_DL_LIST(name)    struct list name = { &(name), &(name) }
/* END: BUGXXXX,modified by zhangheng on 2015-4-21 */

	/**********************************************************
	example function
	**********************************************************/
	//void list_example(void);
#endif  

