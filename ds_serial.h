#include"serial.h"

serial_t *serial;
queue_t *upQueue,*downQueue;
crowd_t *disk_read,*users;

void Init_ds(int ncylinders, int seektime); // ncylinders is total number of disk cylinders 
int Disk_Request(int cylinderno, void* model_request(), int seekedcylinders[], int tid); // cylinderno is requested cylinder. 3rd parameter should return number of cylinders seeked from previous request to service this request. Return value is the sequence number of service order of requests.
