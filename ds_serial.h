void Init_ds(int ncylinders); // ncylinders is total number of disk cylinders 
int servicedseqno = Disk_Request(int cylinderno, void* model_request(), int &seekedcylinders); // cylinderno is requested cylinder. 3rd parameter should return number of cylinders seeked from previous request to service this request. Return value is the sequence number of service order of requests.
