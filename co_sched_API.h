int Init();
int QueTask(volatile void (*task_ptr), unsigned int priority);
int ReRunME(int delay);
int Dispatch();
int decrementAll(int counter);
struct Qnode {
    volatile int priority;
    volatile int sleep_time;
    volatile void (*task_ptr);
    struct Qnode* next;
};
