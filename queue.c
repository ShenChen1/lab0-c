#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

#include "harness.h"
#include "queue.h"

/*
 * Create empty queue.
 * Return NULL if could not allocate space.
 */
queue_t *q_new()
{
    queue_t *q = malloc(sizeof(queue_t));
    /* TODO: What if malloc returned NULL? */
    if (q) {
        q->head = NULL;
        q->tail = NULL;
        q->length = 0;
    }

    return q;
}

/* Free all storage used by queue */
void q_free(queue_t *q)
{
    /* TODO: How about freeing the list elements and the strings? */
    /* Free queue structure */
    if (q == NULL)
        return;

    list_ele_t *node = NULL;
    while (q->head) {
        node = q->head;
        q->head = q->head->next;
        free(node->value);
        free(node);
    }

    free(q);
}

/*
 * Attempt to insert element at head of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_head(queue_t *q, char *s)
{
    list_ele_t *newh;
    /* TODO: What should you do if the q is NULL? */
    if (q == NULL || s == NULL)
        return false;

    newh = malloc(sizeof(list_ele_t));
    if (newh == NULL)
        return false;

    /* Don't forget to allocate space for the string and copy it */
    size_t size = strlen(s) + 1;
    newh->value = malloc(size);
    /* What if either call to malloc returns NULL? */
    if (newh->value == NULL)
        goto err;
    strncpy(newh->value, s, size);

    newh->next = q->head;
    q->head = newh;
    if (q->length == 0)
        q->tail = newh;
    q->length++;
    return true;

err:
    free(newh);
    return false;
}

/*
 * Attempt to insert element at tail of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_tail(queue_t *q, char *s)
{
    /* TODO: You need to write the complete code for this function */
    list_ele_t *new;
    /* TODO: What should you do if the q is NULL? */
    if (q == NULL || s == NULL)
        return false;

    new = malloc(sizeof(list_ele_t));
    if (new == NULL)
        return false;

    /* Don't forget to allocate space for the string and copy it */
    size_t size = strlen(s) + 1;
    new->value = malloc(size);
    /* What if either call to malloc returns NULL? */
    if (new->value == NULL)
        goto err;
    strncpy(new->value, s, size);
    new->next = NULL;

    /* Remember: It should operate in O(1) time */
    /* TODO: Remove the above comment when you are about to implement. */
    if (q->length)
        q->tail->next = new;
    q->tail = new;
    if (q->length == 0)
        q->head = new;
    q->length++;
    return true;

err:
    free(new);
    return false;
}

/*
 * Attempt to remove element from head of queue.
 * Return true if successful.
 * Return false if queue is NULL or empty.
 * If sp is non-NULL and an element is removed, copy the removed string to *sp
 * (up to a maximum of bufsize-1 characters, plus a null terminator.)
 * The space used by the list element and the string should be freed.
 */
bool q_remove_head(queue_t *q, char *sp, size_t bufsize)
{
    /* TODO: You need to fix up this code. */
    /* TODO: Remove the above comment when you are about to implement. */
    if (q == NULL || q->length == 0)
        return false;

    if (sp) {
        strncpy(sp, q->head->value, bufsize);
        sp[bufsize - 1] = '\0';
    }

    list_ele_t *next = q->head->next;
    free(q->head->value);
    free(q->head);
    q->head = next;
    if (q->length == 1)
        q->tail = NULL;
    q->length--;
    return true;
}

/*
 * Return number of elements in queue.
 * Return 0 if q is NULL or empty
 */
int q_size(queue_t *q)
{
    /* TODO: You need to write the code for this function */
    /* Remember: It should operate in O(1) time */
    /* TODO: Remove the above comment when you are about to implement. */
    return q ? q->length : 0;
}

/*
 * Reverse elements in queue
 * No effect if q is NULL or empty
 * This function should not allocate or free any list elements
 * (e.g., by calling q_insert_head, q_insert_tail, or q_remove_head).
 * It should rearrange the existing ones.
 */
void q_reverse(queue_t *q)
{
    /* TODO: You need to write the code for this function */
    /* TODO: Remove the above comment when you are about to implement. */
    if (q == NULL)
        return;

    list_ele_t *newh = NULL;
    list_ele_t *node = q->head;
    list_ele_t *tmp = NULL;

    while (node) {
        tmp = node->next;
        node->next = newh;
        newh = node;
        node = tmp;
    }

    q->tail = q->head;
    q->head = newh;
}

static int _cmp(const void *p1, const void *p2)
{
    const list_ele_t *e1 = *(const list_ele_t **)p1;
    const list_ele_t *e2 = *(const list_ele_t **)p2;

    return strcmp(e1->value, e2->value);
}

/*
 * Sort elements of queue in ascending order
 * No effect if q is NULL or empty. In addition, if q has only one
 * element, do nothing.
 */
void q_sort(queue_t *q)
{
    /* TODO: You need to write the code for this function */
    /* TODO: Remove the above comment when you are about to implement. */
    if (q == NULL || q->length == 0)
        return;

    list_ele_t** array = mmap(NULL,
                              q->length * sizeof(list_ele_t*),
                              PROT_READ | PROT_WRITE,
                              MAP_PRIVATE | MAP_ANONYMOUS,
                              -1,
                              0);
    if (array == NULL)
        return;

    size_t total = 0;
    while (q->head) {
        array[total++] = q->head;
        q->head = q->head->next;
    }

    qsort(array, q->length, sizeof(list_ele_t *), _cmp);

    total = 0;
    while (total < q->length) {
        array[total]->next = array[total + 1];
        total++;
    }
    q->head = array[0];
    q->tail = array[q->length - 1];
    q->tail->next = NULL;

    munmap(array, q->length * sizeof(list_ele_t *));
}
