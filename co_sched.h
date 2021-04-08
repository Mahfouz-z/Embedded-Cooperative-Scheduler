// A queue linked list node

struct Qnode {
    int priority;
    int sleep_time;
    void (*task_ptr);
    struct Qnode* next;
};