#include"serial.h"
#include<pthread.h>
#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>

crowd_t* Create_Crowd(serial_t* serializer)
{
	crowd_t *new_crowd = (struct crowd *)malloc(sizeof(struct crowd));
	pthread_mutex_init(&(new_crowd->lock),NULL);
	new_crowd->count = 0;
	serializer->crowds[serializer->crowd_count++] = new_crowd;
	return new_crowd;
}

int Crowd_Empty(serial_t* serializer, crowd_t* crowd)
{
	if(crowd->count == 0)
		return 1;
	else
		return 0;
}


queue_t* Create_Queue(serial_t* serializer)
{
	queue_t *queue = (struct queue *)malloc(sizeof(struct queue));
	queue->head = NULL;
	queue->tail = NULL;
	pthread_mutex_init(&(queue->lock), NULL);
	serializer->queues[serializer->no_of_queues++] = queue;
	return queue;
}

int Queue_Empty(serial_t* serializer, queue_t* queue)
{
	if(!queue->head && !queue->tail)
	{
		return 1;
	}
	else
		return 0;
}

void enqueue(queue_t *queue, pthread_t id, cond_t (*func)())
{
	struct node *new_node = (struct node *)malloc(sizeof(struct node));
	new_node->id = id;
	new_node->func = func;
	new_node->data = -1;
	new_node->next = NULL;
	pthread_mutex_lock(&(queue->lock));
	if(!queue->head)
	{
		queue->head = new_node;
		queue->tail = new_node;
	}
	else
	{
		queue->tail->next = new_node;
		queue->tail = new_node;
	}
	queue->size++;
	pthread_mutex_unlock(&(queue->lock));
}
void dequeue(queue_t* queue)
{
	pthread_mutex_lock(&(queue->lock));
	struct node *temp = queue->head;
	if(temp->next)
	{
		queue->head = temp->next;
	}
	else
	{
		queue->head = NULL;
		queue->tail = NULL;
	}
	free(temp);
	queue->size--;
	pthread_mutex_unlock(&(queue->lock));
}





serial_t* Create_Serial()
{
	serial_t *new_serial = (struct serializer *)malloc(sizeof(struct serializer));
	new_serial->no_of_queues = 0;
	new_serial->crowd_count = 0;
	pthread_mutex_init(&(new_serial->lock), NULL);
	new_serial->queues[0] = Create_Queue(new_serial);
	new_serial->queues[1]  = Create_Queue(new_serial);
	return new_serial;
}

void Serial_Enter(serial_t* serial)
{
	pthread_t id = pthread_self();
	enqueue(serial->queues[0],id,NULL);
	while(pthread_equal(serial->queues[0]->head->id,id) == 0);
	pthread_mutex_lock(&(serial->lock));
	dequeue(serial->queues[0]);
}

void Serial_Exit(serial_t* serial)
{
	pthread_mutex_unlock(&(serial->lock));
}

void Serial_Enqueue(serial_t* serial,queue_t* queue, cond_t (*func)())
{
	pthread_t tid = pthread_self();
	enqueue(queue,tid,func);
	pthread_mutex_unlock(&(serial->lock));
	while(pthread_equal(queue->head->id, tid)==0);
	pthread_mutex_lock(&(serial->lock));

	cond_t check;
	check = func();
	while(!check)
	{
		check = func();
	}
	dequeue(queue);
}

void Serial_Join_Crowd(serial_t* serial, crowd_t* crowd, void*(*func)())
{
	pthread_mutex_lock(&(crowd->lock));
	crowd->count+=1;
	printf("crowd count at enter %d %u\n",crowd->count,&crowd);
	pthread_mutex_unlock(&(crowd->lock));
	pthread_mutex_unlock(&(serial->lock));
	func();
	pthread_t id = pthread_self();
	pthread_mutex_lock(&(crowd->lock));
	crowd->count-=1;
	printf("crowd count at exit %d %u\n",crowd->count,&crowd);
	pthread_mutex_unlock(&(crowd->lock));
	enqueue(serial->queues[1],id,NULL);
	while(pthread_equal(serial->queues[1]->head->id,id)==0);
	pthread_mutex_lock(&(serial->lock));
	dequeue(serial->queues[1]);
}
