#include"serial.h"

serial_t *serial;
crowd_t *eating_crowd,*thinking_crowd;
queue_t *waiting_queue; 

void Init_dp(int nphilosophers); 
void *Eat(int phil_id, void*(*model_eat)()); 
void *Think(int phil_id,void*(*model_think)());
