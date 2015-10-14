#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#include "rw_serial.h"

long* read_data()
{
	int key = rand() % SIZE;
	usleep(rand() % 10 * DELAY);
	return (long *) database[key];
}

void* write_data()
{
	int key = rand() % SIZE;
	usleep(rand() % 10 * DELAY);
	database[key] = key;
	return NULL;
}

cond_t read_queue_cond()
{
	return Crowd_Empty(serializer, writers_crowd);
}

cond_t write_queue_cond()
{
	return (Crowd_Empty(serializer, readers_crowd) &&
			Crowd_Empty(serializer, writers_crowd));
}

void create()
{
	// Init database
	int i;
	srand(time(NULL));
	for (i = 0; i < SIZE; i++)
		database[i] = rand();
	// Init Serializer
	serializer = Create_Serial();
	waiting_q = Create_Queue(serializer);
	readers_crowd = Create_Crowd(serializer);
	writers_crowd = Create_Crowd(serializer);
}

void *read_func(void *id)
{
	long tid;
	tid = (long)id;
	Serial_Enter(serializer);
	printf("Reader thread #%ld starts!\n", tid);
	Serial_Enqueue(serializer, waiting_q, &read_queue_cond, 0);
	printf("reader thread %ld going to join crowd \n",tid);
	Serial_Join_Crowd(serializer, readers_crowd,(void *) (&read_data));
	//printf("reader thread joins %ld crowd\n",tid);
	Serial_Exit(serializer);
	printf("Reader thread #%ld ends!\n", tid);
	pthread_exit(NULL);
}

void *write_func(void *id)
{
	long tid;
	tid = (long)id;
	Serial_Enter(serializer);
	printf("Writer thread %ld starts!\n", tid);
	Serial_Enqueue(serializer, waiting_q, &write_queue_cond, 0);
	printf("writer thread %ld going to join crowd\n",tid);
	Serial_Join_Crowd(serializer, writers_crowd, &write_data);
	//printf("writer thread %ld joins crowd\n",tid);
	Serial_Exit(serializer);
	printf("Writer thread #%ld ends!\n", tid);
	pthread_exit(NULL);
}
