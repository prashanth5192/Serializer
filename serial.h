#include<pthread.h>


typedef int cond_t;
typedef cond_t (*Function)(void *arg);

struct node
{
	int data;
	Function func;
	pthread_t id;
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
	pthread_mutex_t lock;
	queue_t *queues[100];
	crowd_t *crowds[100];
};

typedef struct serializer serial_t;

serial_t* Create_Serial(); 
void Serial_Enter(serial_t* serial); 
void Serial_Exit(serial_t* serial); 
queue_t *Create_Queue(serial_t* serializer); 
crowd_t *Create_Crowd(serial_t* serializer); 
int Queue_Empty(serial_t* serializer, queue_t* queue); 
int Crowd_Empty(serial_t* serial, crowd_t* crowd); 
void Serial_Enqueue(serial_t* serial, queue_t* queue, cond_t(*func)()); 
void Serial_Join_Crowd(serial_t* serial, crowd_t* crowd, void*(*func)());
void enqueue(queue_t* queue, pthread_t id, cond_t(*func)());
void dequeue(queue_t* queue);

//Declarations of all possible functions//
serial_t Create_Serial(); 
Serial_Enter(serial_t); 
Serial_Exit({serial_t); 
	queue_t Create_Queue(serial_t); 
	crowd_t Create_Crowd(serial_t); 
	int Queue_Empty(serial_t, queue_t); 
	int Crowd_Empty(serial_t, crowd_t); 
	Serial_Enqueue(serial_t, queue_t, cond_t;(*func)()); 
	Serial_Join_Crowd(serial_t,crowd_t,void*(*func)());
