int Init(void);
int QueTask(void (*task_ptr), unsigned int priority);
int ReRunME(int delay);
int Dispatch(void);
void coop_sched_tick(void);
