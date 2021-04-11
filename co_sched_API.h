int Init(void);
int QueTask(void (*task_ptr), unsigned int priority);
int ReRunMe(int delay);
int Dispatch(void);
void coop_sched_tick(void);
