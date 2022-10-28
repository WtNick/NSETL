

#include "tasklet.h"

static struct DLList readylist;
static struct CSLList timeoutcontextlist;
static struct TLWaitContext sleepcontext;
static struct SLQueue isr_deferredqueue;
static int lasttick;
static int processedtick;

#define TLCONTEXT_FLAGS_REGISTERED (1<<0)

/******************************************************************************

   _tasklet_resume
   
   place a task back in the readylist

******************************************************************************/
static void _tasklet_resume(struct TaskLet* tl){
	dllist_insertlast(&readylist, &tl->queueitem);
}

/******************************************************************************

   __tasklet_processTimeout
   
   returns !=0 if the wait context has no more pending tasks

******************************************************************************/
static int __tasklet_processTimeout(struct TLWaitContext* context)
{
	if (context->waitqueue.head){
		// Release all tasklets that are waiting in this context
		// first insert a marker in the wait queue
		// we will start to deplete the queue up to the marker
		// any item that is inserted after will not be released at this point
		struct SLListNode marker;
		slqueue_push(&context->waitqueue, &marker);

		struct SLListNode* node;
		while((node = slqueue_pop(&context->waitqueue))!=&marker) {
			struct TaskLet* tl = LLIST_ITEM(struct TaskLet, node, queueitem);
			if ((tl->pendingticks)&&((--tl->pendingticks)==0)){
				tl->pendingticks=-1; // pendingticks == -1 marks timeout
				_tasklet_resume(tl);
			}else{
				// no timeout, reinsert for next time
				slqueue_push(&context->waitqueue, &tl->queueitem);
			}
		}
	}
	return context->waitqueue.head==NULL;
}

/******************************************************************************

   __tasklet_processTimeout
   
   register the waitcontext for timeout processing if needed

******************************************************************************/
static void waitcontext_registerfortimeoutprocessing(struct TLWaitContext* context){
	if (context->flags & TLCONTEXT_FLAGS_REGISTERED) return;
	sllist_insertfirst(&timeoutcontextlist, &context->timeout_item);
	context->flags |= TLCONTEXT_FLAGS_REGISTERED;
}

/******************************************************************************

    __tasklet_waiton
	
	bind the tasklet on the wait context
	this function is wrapped in the TASKLET_AWAIT macro, 

******************************************************************************/
void __tasklet_waiton(struct TaskLet* tl, struct TLWaitContext* context, int timeout)
{
	dllist_remove(&readylist, &tl->queueitem);

	tl->pendingticks = timeout;
    slqueue_push(&(context)->waitqueue, &(tl->queueitem));

	if (timeout){
		 waitcontext_registerfortimeoutprocessing(context);
	}
}

/******************************************************************************

    __tasklet_sleep
	
	bind the tasklet to the sleep wait context.
	this function is wrapped in the TASKLET_SLEEP macro. 

******************************************************************************/
void __tasklet_sleep(struct TaskLet* tl, int timeout){
	__tasklet_waiton(tl, &sleepcontext, timeout);
}

void __tasklet_destroy(struct TaskLet* tl)
{
	// Nothing yet 
}

/******************************************************************************

    tasklet_release
	
	Release a the pending tasks from a waitcontext.
	

******************************************************************************/
void tasklet_release(struct TLWaitContext* waitcontext){
	CRITICAL_SECTION_ENTER();
	slqueue_push(&isr_deferredqueue, &waitcontext->isr_deferredqueuenode);
	CRITICAL_SECTION_LEAVE();
}


static struct TLWaitContext* ATOM_defqueue_pop()
{
	CRITICAL_SECTION_ENTER();
	struct SLListNode* n =  slqueue_pop(&isr_deferredqueue);
	CRITICAL_SECTION_LEAVE();
	if (n==NULL) return NULL;
	return LLIST_ITEM(struct TLWaitContext, n, isr_deferredqueuenode);
}

/******************************************************************************

   tasklet_releaseall

******************************************************************************/
static void tasklet_releaseall(struct TLWaitContext* context)
{
    // Release all tasklets that are waiting in this context
    // first insert a marker in the wait queue
    // we will start to deplete the queue up to the marker
    // any item that is inserted after will not be released at this point
    struct SLListNode marker;
    slqueue_push(&context->waitqueue, &marker);

    struct SLListNode* node;
    while((node = slqueue_pop(&context->waitqueue))!=&marker) {
        struct TaskLet* tl = LLIST_ITEM(struct TaskLet, node, queueitem);
        //while(tl) {tl = _tasklet_resume(tl);  -- removed this, to be used with TASKLET_INVOKE}
		_tasklet_resume(tl);
    }
}

/******************************************************************************

   tasklet_releasefirst   
   --- Not in use yet, may be used in the future to implement true semaphore functionality 
       with the TLWaitContext. For now.. not used.

******************************************************************************/
static int tasklet_releasefirst(struct TLWaitContext* context)
{
    struct SLListNode* node = slqueue_pop(&context->waitqueue);
    if (node) {
        struct TaskLet* tl = LLIST_ITEM(struct TaskLet, node, queueitem);
        //while(tl) {tl = _tasklet_resume(tl);  -- removed this, to be used with TASKLET_INVOKE}
		_tasklet_resume(tl);
        return 1;
    }
    return 0;
}

/******************************************************************************

	tasklet_init

******************************************************************************/
void tasklet_init(struct TaskLet* tl, struct _TaskLetClass* tlc)
{
	tl->_class = tlc;
	tl->exitto = NULL;
	tl->queueitem.next = NULL;
}

/******************************************************************************

	tasklet_start
	
	Start a task.
	This function places the task structure in the ready list. The task will
	be executed in due time...
	
******************************************************************************/
int tasklet_start(struct TaskLet* tl, void *arg){
	if (tl->_class==NULL) return RESULT_FAILURE;
	if (tl->queueitem.next) return RESULT_FAILURE;
	tl->state = 0;
	tl->arg = arg;
	_tasklet_resume(tl);
	return RESULT_SUCCESS;
}

/******************************************************************************

	isr_tasklet_tick
	
	tasklet tick handler.
	call this function from an timer ISR.
	
******************************************************************************/
void isr_tasklet_tick(void){
	lasttick++;
}

/******************************************************************************

	tasklet_process
	
	Main tasklet processing function.
	Call in a loop: example:
	while(1){
		if (tasklet_process()){
			cpu_idle();
		}
	}
	
******************************************************************************/
int tasklet_process(void){

	struct TLWaitContext* waitcontext;
	while(waitcontext = ATOM_defqueue_pop()){
		tasklet_releaseall(waitcontext);
	}
	
	while(lasttick - processedtick > 0){
		// catch up with the last tick
		SLLIST_FOREACH(c, &timeoutcontextlist);
			struct TLWaitContext* context = LLIST_ITEM(struct TLWaitContext, c, timeout_item);

			if (__tasklet_processTimeout(context))	{
				SLLIST_LOOPREMOVE(c, &timeoutcontextlist);
				context->flags &= ~1;
			}
		SLLIST_NEXT(c);
		processedtick++;
	}

	if (dllist_isempty(&readylist)) return 1;	
	struct TaskLet* tl = LLIST_ITEM(struct TaskLet, dllist_rotate(&readylist), queueitem);
	tl->_class->EntryPoint(tl);
	return 0;
}
