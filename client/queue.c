#include "queue.h"
#include <stdio.h>
#include <stdlib.h>

struct queue
{
  queueNode *begin;
  queueNode *end;
  int size;
};

struct queueNode
{
  char *data;
  queueNode *next;
  queueNode *prev;
};

Queue *create_queue()
{
  Queue *queue = malloc(sizeof(queue));
  if (queue != NULL)
  {
    queue->size = 0;
    queue->begin = NULL;
    queue->end = NULL;
  }
  return queue;
}

int free_queue(Queue *queue)
{
  if (queue == NULL)
    return INVALID_NULL_POINTER;
  queueNode *aux = queue->begin;

  while (aux != NULL)
  {
    queue->begin = aux->next;
    free(aux);
    aux = queue->begin;
  }
  free(queue);
  return SUCCESS;
}

int get_queue(Queue *queue, char *str)
{
  if (queue == NULL)
    return INVALID_NULL_POINTER;
  if (queue->size == 0)
    return OUT_OF_RANGE;
  str = queue->begin->data;
  return SUCCESS;
}

int push(Queue *queue, char *str)
{
  if (queue == NULL)
    return INVALID_NULL_POINTER;
  queueNode *node = malloc(sizeof(queueNode));
  if (node == NULL)
    return OUT_OF_MEMORY;
  node->data = str;
  node->next = NULL;
  if (queue->begin == NULL)
  { 
    queue->begin = node;
    queue->end = node;
    queue->begin->prev = NULL;
  }
  else
  {
    queue->end->next = node;
    node->prev = queue->end;
    queue->end = node;
  }
  queue->size++;
  return SUCCESS;
}

int queue_size(Queue *queue)
{
  if (queue == NULL)
    return INVALID_NULL_POINTER;

  return queue->size;
}

int pop(Queue *queue)
{
  if (queue == NULL)
    return INVALID_NULL_POINTER;
  if (queue->size == 0)
    return OUT_OF_RANGE;
  if (queue->size == 1)
  {
    queueNode *aux = queue->begin;
    queue->begin = NULL;
    free(aux->data);
    free(aux);
    queue->end = NULL;
  }
  else
  {
    queueNode *aux = queue->begin;
    queue->begin = queue->begin->next;
    queue->begin->prev = NULL;
    free(aux);
  }

  queue->size--;
  return SUCCESS;
}