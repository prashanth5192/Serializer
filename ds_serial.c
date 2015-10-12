#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include"ds_serial.h"

int current = 0;
int old = 0;
int servicedseqno = 0;
typedef enum{false=0,true=1} bool;

bool direction;

void Init_ds(int ncylinders, int seektime)
{
        serial = Create_Serial();
        users = Create_Crowd(serial);
        //thinking_crowd = Create_Crowd(serial);
        upQueue = Create_Queue(serial);
        downQueue = Create_Queue(serial);
        direction = true;
}

void disk_release()
{
        if (direction && (Queue_Empty(serial, upQueue)==1))
        {
                printf("downward changing direction\n\n");
                direction = false;
        }
        else if(!direction && (Queue_Empty(serial, downQueue)==1))
        {
                printf("upward changing direction\n\n");
                direction = true;
        }
}

int Disk_Request(int cylinderno, void* model_request(), int *seekedcylinders, int tid)
{
        cond_t cond_upqueue()
        {
                if((Crowd_Empty(serial, users)==1) && ((Queue_Empty(serial, downQueue)==1) || direction))
                {
                        printf("true");
                        return 1;
                }
                else
                        return 0;
        }

        cond_t cond_downqueue()
        {
                if((Crowd_Empty(serial, users)==1) && ((Queue_Empty(serial, upQueue))==1 || !direction))
                {
                        printf("true/****");
                        return 1;
                }
                else
                        return 0;
        }

        void* dummy_request()
        {
                model_request(tid, seekedcylinders[tid]);
        }

        Serial_Enter(serial);

        if(cylinderno > current || (cylinderno == current && !direction))
                Serial_Enqueue(serial, upQueue, &cond_upqueue, cylinderno);
        else
                Serial_Enqueue(serial, downQueue, &cond_downqueue, -(cylinderno));

        old = current;
        current = cylinderno;
        seekedcylinders[tid] = abs(current - old);
        Serial_Join_Crowd(serial, users, &dummy_request);
        printf("seeked cylinders for id %d is %d\n old %d/ current %d\n",tid,seekedcylinders[tid],old,current);
        disk_release();
        servicedseqno+=1;
        Serial_Exit(serial);
        return servicedseqno;
}
