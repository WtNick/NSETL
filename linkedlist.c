
#include "linkedlist.h"

int dllist_isempty(struct DLList* list)
{
	return list->head == 0;
}

int sllist_isempty(struct SLList* list)
{
	return list->head == 0;
}

int csllist_isempty(struct CSLList* list)
{
	return list->head == 0;
}

void slqueue_init(struct SLQueue* slqueue)
{
	slqueue->head = NULL;
	slqueue->tail = NULL;
}
// returns 1 if the queue was empty
int slqueue_push(struct SLQueue* slqueue, struct SLListNode* node)
{
	node->next = NULL;
	if (slqueue->tail == NULL) {
		// First item
		slqueue->head = node;
		slqueue->tail = node;
		return 1;
	} else {
		slqueue->tail->next = node;
		slqueue->tail = node;
		return 0;
	}
}

struct SLListNode* slqueue_pop(struct SLQueue* slqueue)
{
	struct SLListNode* result = slqueue->head;
	if (result == NULL) return NULL;
	slqueue->head = result->next;
	if (result->next == NULL) {
		slqueue->tail = NULL;
	}
	#if LLIST_CLEARDANGLE == 1
	result->next = NULL;
	#endif
	return result;
}



void dllist_init(struct DLList* list)
{
	list->head = NULL;
	list->tail = NULL;
}

void dllist_insertafter(struct DLList* list, struct DLListNode* node, struct DLListNode* newnode)
{
	newnode->prev = node;
	newnode->next = node->next;
	if (node->next == NULL)	{
		list->tail = newnode;
	} else {
		node->next->prev = newnode;
	}
	node->next = newnode;
}

void dllist_insertbefore(struct DLList* list,struct DLListNode* node, struct DLListNode* newnode)
{
	newnode->prev = node->prev;
	newnode->next = node;
	if(node->prev == NULL) {
		list->head = newnode;
	} else {
		node->prev->next = newnode;
	}
	node->prev = newnode;
}

void dllist_insertfirst(struct DLList* list, struct DLListNode* newnode)
{
	if (list->head == NULL)	{
		list->head = newnode;
		list->tail = newnode;
		newnode->prev = NULL;
		newnode->next = NULL;
	} else {
		dllist_insertbefore(list, list->head, newnode);
	}
}

void dllist_insertlast(struct DLList* list, struct DLListNode* newnode)
{
	if (list->tail == NULL)	{
		dllist_insertfirst(list, newnode);
	} else {
		dllist_insertafter(list, list->tail, newnode);
	}
}

void dllist_bringtofront(struct DLList* list, struct DLListNode* node)
{
	dllist_remove(list,node);
	dllist_insertfirst(list,node);
}

void dllist_sendtoback(struct DLList* list, struct DLListNode* node)
{
	dllist_remove(list,node);
	dllist_insertlast(list,node);
}

struct DLListNode* dllist_rotate(struct DLList* list)
{
	struct DLListNode* item = list->head;
	if (item){
		if (list->head == list->tail) return item;

		list->head = item->next;
		list->head->prev = NULL;

		item->prev = list->tail;
		item->next = NULL;
		
		list->tail->next  = item;
		list->tail = item;
	}
	return item;
}


void dllist_remove(struct DLList* list, struct DLListNode* node)
{
	if (node->prev == NULL)	{
		list->head = node->next;
	} else {
		node->prev->next = node->next;
	}

	if (node->next == NULL) {
		list->tail = node->prev;
	} else	{
		node->next->prev = node->prev;
	}
}

struct DLListNode* dllist_popfirst(struct DLList* list)
{
	struct DLListNode* result = list->head;
	if (result)	{
		dllist_remove(list, result);
	}
	return result;
}

int dllist_count(struct DLList* list)
{
	int count = 0;
	struct DLListNode* cursor = list->head;
	while(cursor) {
		count++;
		cursor = cursor->next;
	}
	return count;
}


void sllist_init(struct SLList* list)
{
	list->head = NULL;
}

void sllist_insertafter(struct SLList* list, struct SLListNode* node, struct SLListNode* newnode)
{
	newnode->next = node->next;
	node->next = newnode;
}

void sllist_insertfirst(struct SLList* list, struct SLListNode* newnode)
{
	newnode->next = list->head;
	list->head = newnode;
}

struct SLListNode* sllist_popfirst(struct SLList* list)
{
	struct SLListNode* result = list->head;
	if (result)	{
		list->head = result->next;
		
		// clear dangling pointer
		result->next = NULL;
	}
	return result;
}

// [NOTE] O(n)
int sllist_remove(struct SLList* list, struct SLListNode* node)
{
	ASSERT(list->head,"Cannot remove the node from an empty list.")
	// first find the node linking to 'node'
	struct SLListNode* cursor = list->head;
	
	if (node == cursor)	{
		list->head = node->next;
		node->next = 0;
		return RESULT_SUCCESS;
	}
	struct SLListNode* prev = cursor;

	for(cursor = cursor->next;cursor != NULL;cursor = cursor->next)	{
		if (cursor ==  node) {
			prev->next = node->next;
			node->next = 0;
			return RESULT_SUCCESS;
		}
		prev = cursor;
	};
	return RESULT_FAILURE;
	// Node does not belong to the list
}

int sllist_count(struct SLList* list)
{
	int count = 0;
	struct SLListNode* cursor = list->head;
	while(cursor) {
		count++;
		cursor = cursor->next;
	}
	return count;
}

void sllist_bringtofront(struct SLList* list, struct SLListNode* node)
{
	// don't do anything if node is already at top
	if (list->head == node) return;
	
	struct SLListNode* cursor = list->head;

	struct SLListNode* prev = NULL;
	while(cursor != node) {
		prev = cursor;
		cursor = cursor->next;
	}
	if (prev) {
		prev->next = cursor->next;
	} else {
		// first item
		list->head = cursor->next;
	}
	
	// find the top of the list
	cursor = list->head;
	while(cursor->next)	{
		cursor = cursor->next;
	}
	
	// if not already top of the list, put it there
	if (cursor != node)	{
		cursor->next = node;
		node->next = NULL;
	}
}

void csllist_init(struct CSLList* list)
{
	list->head = NULL;
}

void csllist_insertafter(struct CSLList* list,
struct SLListNode* node,
struct SLListNode* newnode)
{
	newnode->next = node->next;
	node->next = newnode;
}

void csllist_insertfirst(struct CSLList* list, struct SLListNode* newnode)
{
	if (list->head)	{
		newnode->next = list->head->next;
		list->head->next = newnode;
		list->head = newnode;
	} else {
		newnode->next = newnode;
	}
	list->head = newnode;
}

void csllist_rotate(struct CSLList* list)
{
	if (list->head){list->head = list->head->next;}
}

// Remark: this function has O(n) execution time
//         so only use for small lists
void csllist_remove(struct CSLList* list, struct SLListNode* node)
{
	ASSERT(list->head,"Cannot remove the node from an empty list.")
	// first find the node linking to 'node'
	struct SLListNode* cursor = list->head;
	do {
		if (cursor->next == node) break;
		cursor = cursor->next;
	}while(cursor);
	
	if (cursor == node)	{
		// this was the last node in the list
		list->head = NULL;
	} else{
		cursor->next = node->next;
	}
	
	// clear dangling pointer
	node->next = NULL;
}

int csllist_count(struct CSLList* list)
{
	int count = 0;
	struct SLListNode* cursor = list->head;
	if (cursor) {
		do {
			count++;
			cursor = cursor->next;
		} while(cursor!=list->head);
	}
	return count;
}


struct SLListNode* slqueue_poptail(struct SLQueue* slqueue)
{
	struct SLListNode* result = NULL;
	if (slqueue->tail) 	{
		result = slqueue->tail;
		if (slqueue->head == slqueue->tail) {
			slqueue->head = NULL;
			slqueue->tail = NULL;
		} else {
			// find previous
			struct SLListNode* c = slqueue->head;
			while(c->next != result){c = c->next;}
			// readjust tail
			slqueue->tail = c;
			c->next = NULL;
		}
		result->next = NULL;
	}
	return result;
}
