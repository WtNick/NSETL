/******************************************************************************

   Linked List Library

   (C) January 2009
   Nick Witvrouwen

-------------------------------------------------------------------------------

    This lib provides linked list functionality:

      usage example:

         struct singlelinktest
         {
             int a;
             struct SLListNode* list1; // a single linked list this item belongs to
             struct DLListNode* list2; // a doubly linked list this item belongs to
         };


    struct SLList mylist;
    struct SomeItem
    {
        int value;
        SLListNode mylistentry; // SomeItem entry for list1
        SLListNode otherlist; // SomeItem entry for list2
    };

    // Example: loop through all elements of the linked list
    SLLIST_FOREACH(c, &mylist);
        // get the pointer to the list item using LLIST_ITEM
        struct SomeItem* item = LLIST_ITEM(struct SomeItem, c, mylistentry);

        if (condition)
        {
             SLLIST_LOOPREMOVE(c, &AnimatedList);
        }

    SLLIST_NEXT(c);


*******************************************************************************

 * NOTE!!!

     - C loop 'continue' statement is invalid in the context
       of a FOREACH construct


******************************************************************************/
#ifndef _LINKEDLIST_H_
#define _LINKEDLIST_H_

#include "arch.h"


// 1 indicates that dangling pointers are automatically cleared
#define LLIST_CLEARDANGLE 1


#define LLIST_ITEM(type, listobj, listname) ((type *)((size_t)(listobj) - offsetof(type, listname)))

// Doubly linked list ---------------------------------------------------------
struct DLListNode
{
    struct DLListNode* prev;
    struct DLListNode* next;
};

struct DLList
{
    struct DLListNode* head;
    struct DLListNode* tail;
};

#define DLLIST_FOREACH(cursor, dllist) \
{\
struct DLListNode* cursor; \
for(cursor = (dllist)->head;cursor != NULL;cursor = cursor->next){
#define DLLIST_NEXT(cursor) }}

void dllist_init(struct DLList* list);
int dllist_isempty(struct DLList* list);
int  dllist_count(struct DLList* list);
void dllist_remove(struct DLList* list, struct DLListNode* node);
struct DLListNode* dllist_popfirst(struct DLList* list);
struct DLListNode* dllist_rotate(struct DLList* list);
void dllist_insertlast(struct DLList* list, struct DLListNode* newnode);
void dllist_insertfirst(struct DLList* list, struct DLListNode* newnode);
void dllist_insertbefore(struct DLList* list,struct DLListNode* node, struct DLListNode* newnode);
void dllist_insertafter(struct DLList* list, struct DLListNode* node, struct DLListNode* newnode);
void dllist_bringtofront(struct DLList* list, struct DLListNode* node);
void dllist_sendtoback(struct DLList* list, struct DLListNode* node);

// Single linked list ---------------------------------------------------------
struct SLListNode
{
    struct SLListNode* next;
};

struct SLList
{
    struct SLListNode* head;
};


#define SLLIST_LOOPPREV(cursor) __ ## cursor ## _ ## prev
#define SLLIST_DIRTYMARKER(cursor) __ ## cursor ## _ ## dirty
#define SLLIST_FOREACH(cursor, sllist) \
{\
struct SLListNode* cursor = (sllist)->head; \
struct SLListNode* SLLIST_LOOPPREV(cursor) = NULL; \
int SLLIST_DIRTYMARKER(cursor) = 0; \
while(cursor != NULL)\
{\

#define SLLIST_LOOPREMOVE(cursor, list) \
if (SLLIST_LOOPPREV(cursor))\
{\
    SLLIST_LOOPPREV(cursor)->next = cursor->next;\
}\
else\
{\
    (list)->head = cursor->next;\
}\
struct SLListNode* __rm_ ## cursor = cursor;\
cursor = cursor->next;\
if (LLIST_CLEARDANGLE){__rm_ ## cursor->next = NULL;};\
SLLIST_DIRTYMARKER(cursor) = 1;

#define SLLIST_NEXT(cursor)\
SLLIST_LOOPPREV(cursor) = SLLIST_DIRTYMARKER(cursor)?SLLIST_LOOPPREV(cursor):cursor;\
cursor = SLLIST_DIRTYMARKER(cursor)?cursor:cursor->next;\
SLLIST_DIRTYMARKER(cursor)=0;\
}}

void sllist_init(struct SLList* list);
int sllist_isempty(struct SLList* list);
void sllist_insertafter(struct SLList* list, struct SLListNode* node, struct SLListNode* newnode);
void sllist_insertfirst(struct SLList* list, struct SLListNode* newnode);
struct SLListNode* sllist_popfirst(struct SLList* list);

int  sllist_remove(struct SLList* list, struct SLListNode* node);// O(n)
int  sllist_count(struct SLList* list);// O(n)
void sllist_bringtofront(struct SLList* list, struct SLListNode* node);// O(n)

// Circular linked list -------------------------------------------------------
struct CSLList
{
    struct SLListNode* head;
};

void csllist_init(struct CSLList* list);
int csllist_isempty(struct CSLList* list);
void csllist_insertafter(struct CSLList* list,
                         struct SLListNode* node,
                         struct SLListNode* newnode);
void csllist_insertfirst(struct CSLList* list, struct SLListNode* newnode);
void csllist_remove(struct CSLList* list, struct SLListNode* node);
int csllist_count(struct CSLList* list);
void csllist_rotate(struct CSLList* list);
	
// Single linked Queue -------------------------------------------------------
struct SLQueue
{
    struct SLListNode* head;
    struct SLListNode* tail;
};

void slqueue_init(struct SLQueue* slqueue);
// returns 1 if the queue was empty
int slqueue_push(struct SLQueue* slqueue, struct SLListNode* node);
struct SLListNode* slqueue_pop(struct SLQueue* slqueue);
struct SLListNode* slqueue_poptail(struct SLQueue* slqueue);// O(n)
struct SLListNode* slqueue_peek(struct SLQueue* slqueue);
struct SLListNode* slqueue_peektail(struct SLQueue* slqueue);

#endif //_LINKEDLIST_H_
