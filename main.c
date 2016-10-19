#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdatomic.h>

#include "thread.h"
#include "list.h"

#include "_time.h"

#define USAGE "usage: ./sort [thread_count] [input_count]\n"

struct {
    //pthread_mutex_t mutex;
    _Atomic int cuthread_count;
} thread_data;

struct {
    pthread_mutex_t mutex;//FIXME: No use in code
    llist_t *list;
} tmp_list;

static llist_t *the_list = NULL;
static int thread_count = 0, data_count = 0, max_cut = 0;
static tpool_t *pool = NULL;

llist_t *merge_list(llist_t *a, llist_t *b)
{
    llist_t *new_list = list_new();
    node_t *cur = NULL;
    while (a->size && b->size) {
        llist_t *small_list = (llist_t *)
                              ((intptr_t) a * (a->head->data <= b->head->data) +
                               (intptr_t) b * (a->head->data > b->head->data));
        if (cur) {
            cur->next = small_list->head;
            cur = cur->next;
        } else {
            new_list->head = small_list->head;
            cur = new_list->head;
        }
        small_list->head = small_list->head->next;
        --small_list->size;
        ++new_list->size;
        cur->next = NULL;
    }
    llist_t *remaining_list = (llist_t *) ((intptr_t) a * (a->size > 0) +
                                           (intptr_t) b * (b->size > 0));
    cur->next = remaining_list->head;
    new_list->size += remaining_list->size;
    free(a);
    free(b);
    return new_list;
}

llist_t *merge_sort(llist_t *list)
{
    if (list->size < 2)
        return list;
    int mid = list->size / 2;
    llist_t *newlist = list_new();
    newlist->head = list_get(list, mid);
    newlist->size = list->size - mid;
    list_get(list, mid - 1)->next = NULL;
    list->size = mid;
    return merge_list(merge_sort(list), merge_sort(newlist));
}

void merge(void *data)
{
    llist_t *list = (llist_t *) data;
    if (list->size < data_count) {
        //pthread_mutex_lock(&(thread_data.mutex));FIXME:No need lock
        llist_t *tmpLocal = tmp_list.list;
        if (!tmpLocal) {
            tmp_list.list = list;
            //pthread_mutex_unlock(&(thread_data.mutex));
        } else {
            tmp_list.list = NULL;
            //pthread_mutex_unlock(&(thread_data.mutex));
            task_t *new_task = (task_t *) malloc(sizeof(task_t));
            new_task->func = merge;
            new_task->arg = merge_list(list, tmpLocal);
            tqueue_push(pool->queue, new_task);
        }
    } else {
        the_list = list;
        task_t *new_task = (task_t *) malloc(sizeof(task_t));
        new_task->func = NULL;
        tqueue_push(pool->queue, new_task);
        list_print(list);
    }
}

void cut(void *data)
{
    llist_t *list = (llist_t *) data;
    //pthread_mutex_lock(&(thread_data.mutex));
    int cutLocal = thread_data.cuthread_count;
    if (list->size > 1 && cutLocal < max_cut) {
        thread_data.cuthread_count++;//++i no effect for _Atomic var
        //pthread_mutex_unlock(&(thread_data.mutex));
        /* Cut list */
        int mid = list->size / 2;
        llist_t *newlist = list_new();
        newlist->head = list_get(list, mid);
        newlist->size = list->size - mid;
        list_get(list, mid-1)->next = NULL;
        list->size = mid;

        /* create new task */
        task_t *new_task = (task_t *) malloc(sizeof(task_t));
        new_task->func = cut;
        new_task->arg = newlist;
        tqueue_push(pool->queue, new_task);
        new_task = (task_t *) malloc(sizeof(task_t));
        new_task->func = cut;
        new_task->arg = list;
        tqueue_push(pool->queue, new_task);
    } else {
        //pthread_mutex_unlock(&(thread_data.mutex));
        merge(merge_sort(list));
    }
}

/**
 * pop the queue for task to run
 */
static void *task_run(void *data)
{
    task_t *current_task = NULL;
    while (1) {
        current_task = tqueue_pop(pool->queue);
        if (current_task) {
            if (!current_task->func) {///no task
                tqueue_push(pool->queue, current_task);///task push
                break;
            } else {
                current_task->func(current_task->arg);
                free(current_task);
            }
        }
    }
    pthread_exit(NULL);
}

int main(int argc, char const *argv[])
{
    if (argc < 3) {
        printf(USAGE);
        return -1;
    }
    thread_count = atoi(argv[1]);
    data_count = atoi(argv[2]);
    max_cut = thread_count * (thread_count <= data_count) +
              data_count * (thread_count > data_count) - 1;

    /* Read data */
    the_list = list_new();

    /* FIXME: remove all all occurrences of printf and scanf
     * in favor of automated test flow.
     */
#ifdef TIMING
    srand(time(NULL));
    for(int i = 0; i < data_count; ++i)
        list_add(the_list, rand());//FIXME: rand only return `int`, we want `long int`
#else
    printf("input unsorted data line-by-line\n");
    for (int i = 0; i < data_count; ++i) {
        long int data;
        scanf("%ld", &data);
        list_add(the_list, data);
    }
#endif

    /*cal time*/
    struct timespec start, end;
    //gettime(&start);

    /* initialize and execute tasks from thread pool */
    //pthread_mutex_init(&(thread_data.mutex), NULL);
    thread_data.cuthread_count = ATOMIC_VAR_INIT(0);
    tmp_list.list = NULL;
    pool = (tpool_t *) malloc(sizeof(tpool_t));
    tpool_init(pool, thread_count, task_run);

    /* launch the first task */
    task_t *new_task = (task_t *) malloc(sizeof(task_t));
    new_task->func = cut;
    new_task->arg = the_list;
    gettime(&start);//
    tqueue_push(pool->queue, new_task);

    /* release thread pool */
    tpool_free(pool);

    gettime(&end);
    printf("mergesort for %d data with %d threads take %ld us\n", data_count, thread_count, diff_in_us(start, end));
    return 0;
}
