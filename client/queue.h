#define SUCCESS 0
#define INVALID_QUEUE_NULL_POINTER -1
#define OUT_OF_MEMORY -2
#define OUT_OF_RANGE -3
#define ELEM_NOT_FOUND -4
#define INVALID_POSITION -5
#define INVALID_NULL_POINTER -6
#define SUCCESS 0

typedef struct queue Queue;
typedef struct queueNode queueNode;

Queue *create_queue();
int free_queue(Queue *queue);
int get_queue(Queue *queue, char *str);
int push(Queue *queue, char *str);
int queue_size(Queue *queue);
int pop(Queue *queue);