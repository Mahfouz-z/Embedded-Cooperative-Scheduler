// A queue linked list node
struct Qnode {
    short priority;
    short sleep_time;
    void (*task_ptr);
    struct Qnode* next;
};

int decrementAll(int counter);
