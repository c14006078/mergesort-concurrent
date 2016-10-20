#include "thread.h"
#include <stdio.h>

int task_free(task_t *the_task)
{
    free(the_task->arg);
    free(the_task);
    return 0;
}

int tqueue_init(tqueue_t *the_queue)
{
    the_queue->head = NULL;
    the_queue->tail = NULL;
    pthread_mutex_init(&(the_queue->mutex), NULL);
    pthread_cond_init(&(the_queue->cond), NULL);
    the_queue->size = 0;
    return 0;
}

task_t *tqueue_pop(tqueue_t *the_queue)
{
    task_t *ret;
    pthread_mutex_lock(&(the_queue->mutex));
    ret = the_queue->tail;
    if (ret) {
        the_queue->tail = ret->last;
        if (the_queue->tail) {
            the_queue->tail->next = NULL;
        } else {
            the_queue->head = NULL;
        }
        the_queue->size--;
    }
    pthread_mutex_unlock(&(the_queue->mutex));
    return ret;
}

uint32_t tqueue_size(tqueue_t *the_queue)
{
    uint32_t ret;
    pthread_mutex_lock(&(the_queue->mutex));
    ret = the_queue->size;
    pthread_mutex_unlock(&(the_queue->mutex));
    return ret;
}

int tqueue_push(tqueue_t *the_queue, task_t *task)
{
    pthread_mutex_lock(&(the_queue->mutex));
    task->last = NULL;
    task->next = the_queue->head;
    if (the_queue->head)
        the_queue->head->last = task;
    the_queue->head = task;
    if (the_queue->size++ == 0)
        the_queue->tail = task;
    pthread_mutex_unlock(&(the_queue->mutex));
    return 0;
}

int tqueue_free(tqueue_t *the_queue)
{
    task_t *cur = the_queue->head;
    while (cur) {
        the_queue->head = the_queue->head->next;
        free(cur);
        cur = the_queue->head;
    }
    pthread_mutex_destroy(&(the_queue->mutex));
    return 0;
}
void *dispatch(void *data)
{
		
	task_t *current_task = NULL;
	tpool_t *mypool = (tpool_t *)data;
	int count = 0 ;
	int deadcount =0;
	
    while (1) {
        current_task = tqueue_pop(mypool->disqueue);
        if (current_task) {
            if (!current_task->func) {
                //tqueue_push(pool->queue, current_task);					
				tqueue_push(mypool->queue[count],current_task);
                count=(count+1)%(mypool->count);
				deadcount++;
				if(deadcount == mypool->count)
					break;
            } else {
                //current_task->func(current_task->arg);
                //free(current_task);
				tqueue_push(mypool->queue[count],current_task);
				count=(count+1)%(mypool->count);
            }
        }
    }
    pthread_exit(NULL);
}


int tpool_init(tpool_t *the_pool, uint32_t tcount, void *(*func)(void *))
{
	    
	the_pool->threads = (pthread_t *) malloc(sizeof(pthread_t) * tcount);
    the_pool->count = tcount;
    the_pool->queue = (tqueue_t **) malloc(tcount*sizeof(tqueue_t *));
    

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    for (int i = 0; i < tcount; ++i)
    {
		the_pool->queue[i] = (tqueue_t *) malloc(sizeof(tqueue_t));	
		tqueue_init(the_pool->queue[i]);
	    pthread_create(&(the_pool->threads[i]), &attr, func, the_pool->queue[i]);
		
	}
	
	the_pool->disqueue = (tqueue_t *) malloc(sizeof(tqueue_t));
	tqueue_init(the_pool->disqueue);
	the_pool->disthread = (pthread_t *) malloc(sizeof(pthread_t) * tcount);
	pthread_create(the_pool->disthread,&attr,dispatch,the_pool);
	
    pthread_attr_destroy(&attr);
    return 0;
}

int tpool_free(tpool_t *the_pool)
{
    for (int i = 0; i < the_pool->count; ++i)
    {
	    pthread_join(the_pool->threads[i], NULL);
		tqueue_free(the_pool->queue[i]);
	}
    free(the_pool->threads);
	free(the_pool->disthread);
	free(the_pool->disqueue);
    
    return 0;
}
