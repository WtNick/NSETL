/*
	Nick's Super Easy Tasklet Library
	---------------------------------
	
	This library allows for super easy creation of co-routines style tasks
	a task class is defined as follows:
	
	TASKLET_DEFINE(mytaskclassname, void* arg)
	   // here you can define local variables
	   // the tasklet engine allows each task class to have a local data frame
	   // that can be used in the tasklet body
	   //for example:
	   // int a;
	   // int b;
	   // ...
	TASKLET_BEGIN(mytaskclassname)
		// task body 
		// here you can use the following tasklet constructs:
		// TASKLET_YIELD()  --> yield execution to another task
		// TASKLET_SLEEP(nr_ofticks) --> bring the task to sleep for an certain amount of ticks
		// TASKLET_AWAIT(&waitcontext) --> wait until a condition becomes true
		// TASKLET_AWAIT_TIMEOUT(&waitcontext, 1000) --> same as AWAIT, but allows a timeout value
		// in the tasklet body, you can refer to variables from the local frame using the 'self' reference
		// example: 
		//    printf("value of a = %i\n", self->a);
	TASKLET_END
	
	use the following construct to allocate memory for a task:
	TASKLET_INSTANCE(mytaskclassname) taskinstance1;
	
 */ 

#ifdef __cplusplus
extern "C" {
#endif
#ifndef _TASKLET_H_
#define _TASKLET_H_

#include "linkedlist.h"

struct TLWaitContext
{
	struct SLListNode timeout_item;
	struct SLQueue waitqueue;
	struct SLListNode isr_deferredqueuenode;
	int flags;
};

struct TaskLet;
struct _TaskLetClass;
void __tasklet_destroy(struct TaskLet* tl);

#define TASKLET(taskname) struct _TaskLetClass taskname
#define TASKLET_DEFINE(taskname, argname) \
__TASKLET_INSTANCE_DEF(taskname);\
struct _TaskLetClass taskname;static __TASKENTRYPROTO(taskname); __TASKLET_CLASS(taskname);__TASKLET_LOCALFRAMETYPE(taskname){argname;
#define TASKLET_BEGIN(taskname) };static __TASKENTRYPROTO(taskname){__TASKLET_LOCALFRAMETYPE(taskname)* __TASKLET_LOCALFRAMENAME = (__TASKLET_LOCALFRAMETYPE(taskname)*)&__tl->arg;switch(__tl->state){case 0:
#define TASKLET_AWAIT(context) __tasklet_waiton(__tl, context, 0);__tl->state = __LINE__;return NULL;case __LINE__:
#define TASKLET_AWAIT_TIMEOUT(context, timeout) __tasklet_waiton(__tl, context, timeout);__tl->state = __LINE__;return NULL;case __LINE__:
#define TASKLET_SLEEP(timeout) __tasklet_sleep(__tl, timeout);__tl->state = __LINE__;return NULL;case __LINE__:
#define TASKLET_INVOKE(taskname, arg) __tl->state = __LINE__;return _tasklet_call(__tl, &taskname, arg);case __LINE__:
#define TASKLET_YIELD()  __tl->state = __LINE__;return NULL;case __LINE__:
#define TASKLET_END }__TASKEXIT;}

/*****************************************************************************/
#define __TASKLET_LOCALFRAMENAME self
#define __TASKLET_LOCALFRAMETYPE(taskname) struct _ ## taskname ## _localframe
#define __TASKENTRYPOINT(taskname) _task_## taskname ## _entrypoint
#define __TASKENTRYPROTO(taskname) struct TaskLet* __TASKENTRYPOINT(taskname)(struct TaskLet* __tl)
#define __TASKLET_CLASS(taskname) struct _TaskLetClass taskname ={.EntryPoint = __TASKENTRYPOINT(taskname)}
#define __TASKEXIT {struct TaskLet* exittask = __tl->exitto;__tasklet_destroy(__tl);return exittask;}
struct _TaskLetClass
{
	struct TaskLet* (*EntryPoint)(struct TaskLet*);
};

struct TaskLet
{
	const struct _TaskLetClass* _class;
	struct DLListNode queueitem;
	int pendingticks;
	struct TaskLet* exitto; // <-- currently not used -> needed as return path from 'call' to external tasklets
	int state;
	void* arg;
};

#define TASKLET_INSTANCE(taskname) struct TaskLet ## taskname

#define __TASKLET_INSTANCE_DEF(taskname) TASKLET_INSTANCE(taskname)\
{\
	struct TaskLet base;\
	__TASKLET_LOCALFRAMETYPE(taskname)\
};\
void _tasklet_waiton(struct TaskLet* tl, struct TLWaitContext* context, int timeout);
void _tasklet_sleep(struct TaskLet* tl, int timeout);


void tasklet_init(struct TaskLet* tl, struct _TaskLetClass* tlc);
int tasklet_start(struct TaskLet* tl, void* arg);
void tasklet_release(struct TLWaitContext* waitcontext);
//struct TaskLet* _tasklet_invoke(struct TaskLet* tl, struct _TaskLetClass* tlc, void* arg);
int tasklet_process(void);
void isr_tasklet_tick(void);



#endif //_TASKLET_H_
#ifdef __cplusplus
}
#endif
