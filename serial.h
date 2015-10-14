#include<pthread.h>
#include<semaphore.h>

typedef int cond_t;
typedef cond_t (*Function)();

struct node
{
	int prio;
	Function func;
	pthread_t id;
	sem_t mutex;
	struct node *next;
};

struct queue
{
	struct node *head, *tail;
	pthread_mutex_t lock;
	int size;
};

struct crowd
{
	int count;
	pthread_mutex_t lock;
};

typedef struct queue queue_t;
typedef struct crowd crowd_t;

struct serializer
{
	int no_of_queues;
	int crowd_count;
	sem_t queue_lock;
	queue_t *queues[100];
	crowd_t *crowds[100];
};

typedef struct serializer serial_t;

serial_t* Create_Serial(); 
void Serial_Enter(serial_t* serial); 
void Serial_Exit(serial_t* serial); 
void Serial_Enqueue(serial_t* serial, queue_t* queue, cond_t(*func)(), int prio);
void Serial_Join_Crowd(serial_t* serial, crowd_t* crowd, void*(*func)());
void leave_serializer(serial_t* serial);

crowd_t *Create_Crowd(serial_t* serializer); 
int Crowd_Empty(serial_t* serial, crowd_t* crowd); 

queue_t *Create_Queue(serial_t* serializer); 
queue_t* enqueue(queue_t* queue, pthread_t id, cond_t(*func)(), int prio);
void dequeue(queue_t* queue);
int Queue_Empty(serial_t* serializer, queue_t* queue); 

