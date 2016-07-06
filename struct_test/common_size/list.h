/*********************************************************************************
*Copyright(C),2010-2020, 西安TCL软件开发有限公司
*FileName:  List.h
*Author:	Li Peng
*Version:	V0.1
*Date:  //完成日期
*Description: 定义了链表的定义以及对应的操作
*History:  
1.Date:		2015.03.23
Author:		Li Peng
Modification:	初始版本
**********************************************************************************/

#ifndef _LIST_H_  
#define _LIST_H_  

/* BEGIN: BUGXXXX,modified by zhangheng for XXXREASONXXX on 2015-4-21 */
#ifndef NULL
#define NULL  ((void *)0)
#endif
/* END: BUGXXXX,modified by zhangheng on 2015-4-21 */

/**********************************************************
功能: 计算MEMBER成员在TYPE结构体中的偏移量
**********************************************************/
#define offsetof(TYPE, MEMBER)  (unsigned long)(&(((TYPE*)0)->MEMBER))  


/**********************************************************
功能: 计算链表元素的起始地址
输入:
ptr：   type结构体中的链表指针
type:   结构体类型
member: 链表成员名称
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
// 将new_link插入到link之前  
static inline void list_insert(struct list *link, struct list *new_link)
{
	new_link->prev = link->prev;
	new_link->next = link;
	new_link->prev->next = new_link;
	new_link->next->prev = new_link;
}

/**********************************************************
功能: 将new_link节点附加到list链表中
**********************************************************/
static inline void list_append(struct list *list, struct list *new_link)
{
	list_insert(list, new_link);
}

//dl_list_del  
/**********************************************************
功能: 从链表中移除节点
**********************************************************/
static inline void list_remove(struct list *link)
{
	link->prev->next = link->next;
	link->next->prev = link->prev;
}

/**********************************************************
获取link节点对应的结构体变量地址
link:   链表节点指针
type:   结构体类型名
member: 结构体成员变量名
**********************************************************/
#define list_entry(link, type, member)  container_of(link, type, member)  

/**********************************************************
获取链表头节点对应的结构体变量地址
list:   链表头指针
type:   结构体类型名
member: 结构体成员变量名
Note:
链表头节点实际为链表头的下一个节点,链表头未使用，相当于哨兵
**********************************************************/
#define list_head(list, type, member) list_entry((list)->next, type, member)  

/**********************************************************
获取链表尾节点对应的结构体变量地址
list:   链表头指针
type:   结构体类型名
member: 结构体成员变量名
**********************************************************/
#define list_tail(list, type, member) list_entry((list)->prev, type, member)  

/**********************************************************
返回链表下一个节点对应的结构体指针
elm:    结构体变量指针
type:   结构体类型名
member: 结构体成员变量名(链表变量名)
**********************************************************/
#define list_next(elm,type,member) list_entry((elm)->member.next, type, member)  

/**********************************************************
遍历链表所有节点对应的结构体
pos : 结构体指针
type : 结构体类型名
list : 链表头指针
member : 结构体成员变量名(链表变量名)
Note : 链表头未使用，因此遍历结束后，pos指向的不是有效的结构体地址
**********************************************************/
//#define list_for_each_entry(pos, type, list, member)    \  
/*for (pos = list_head(list, type, member);               \
	&pos->member != (list);                              \
	pos = list_next(pos, type, member))*/


/* BEGIN: BUGXXXX,modified by zhangheng for XXXREASONXXX on 2015-4-21 */
/* 如下宏均移植于zigbee项目，因数据库原因需要移植 */

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

