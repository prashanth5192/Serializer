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

queue_t* enqueue(queue_t *queue, pthread_t id, cond_t (*func)(), int prio)
{
	struct node *new_node = (struct node *)malloc(sizeof(struct node));
	new_node->id = id;
	new_node->func = func;
	new_node->next = NULL;
	new_node->prio = prio;
	sem_init(&new_node->mutex,0 ,0);		
	pthread_mutex_lock(&(queue->lock));
	if(!queue->head)
	{
		queue->head = new_node;
		queue->tail = new_node;
	}
	else if(new_node->prio >= queue->tail->prio)
	{
		queue->tail->next = new_node;
		queue->tail = new_node;
	}

	else if(new_node->prio <= queue->head->prio)
	{
		new_node->next = queue->head;
		queue->head = new_node;
	}
	else
	{
		struct node *current = queue->head;
		struct node *previous = NULL;
		while(current)
		{
			if(new_node->prio <= current->prio)
			{
				new_node->next = current;
				previous->next = new_node;
				break;
			}
			previous = current;
			current = current->next;
		}
	}
	queue->size++;
	pthread_mutex_unlock(&(queue->lock));
	return new_node;
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
	int i=0;
	serial_t *new_serial = (struct serializer *)malloc(sizeof(struct serializer));
	new_serial->no_of_queues = 0;
	new_serial->crowd_count = 0;
	new_serial->queues[0] = Create_Queue(new_serial);
	new_serial->queues[1]  = Create_Queue(new_serial);
	sem_init(&(new_serial->queue_lock),0,1);
	return new_serial;
}

void Serial_Enter(serial_t* serial)
{
	pthread_t id = pthread_self();
		struct node *new_node = enqueue(serial->queues[0],id,NULL, 0);
	while(pthread_equal(serial->queues[0]->head->id,id) == 0);
	printf("waiting to enter serializer\n\n");
	sem_wait(&serial->queue_lock);
	printf("entered seriaizer\n\n");
	dequeue(serial->queues[0]);
}

void Serial_Exit(serial_t* serial)
{
	leave_serializer(serial);
}

void Serial_Enqueue(serial_t* serial,queue_t* queue, cond_t (*func)(), int prio)
{
	pthread_t tid = pthread_self();
	struct node *new_node = enqueue(queue, tid, func, prio);
	leave_serializer(serial);//
	printf("wating to comne out of queue\n");
	sem_wait(&new_node->mutex);
	dequeue(queue);
}

void Serial_Join_Crowd(serial_t* serial, crowd_t* crowd, void*(*func)())
{
	pthread_mutex_lock(&(crowd->lock));
	crowd->count+=1;
	printf("crowd count at enter %d %u\n",crowd->count,&crowd);
	pthread_mutex_unlock(&(crowd->lock));
	leave_serializer(serial);
	func();
	pthread_t id = pthread_self();
	pthread_mutex_lock(&(crowd->lock));
	crowd->count-=1;
	printf("crowd count at exit %d %u\n",crowd->count,&crowd);
	pthread_mutex_unlock(&(crowd->lock));
		struct node *new_node = enqueue(serial->queues[1],id,NULL, 0);
	while(pthread_equal(serial->queues[1]->head->id,id)==0);
	sem_wait(&serial->queue_lock);
	dequeue(serial->queues[1]);
}

void leave_serializer(serial_t* serial)
{
	int i;
	for(i=2;i<serial->no_of_queues;i++)
	{
		if(Queue_Empty(serial, serial->queues[i])==0)
		{
			cond_t condition = serial->queues[i]->head->func();
			if(condition == 1)
			{ 
				printf("condition satisfied\n");
				sem_post(&serial->queues[i]->head->mutex);
				return;
			}
		}
	}
	sem_post(&serial->queue_lock);
}
