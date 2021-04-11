// A queue array entry

struct Qnode {
    int priority;
    int sleep_time;
    void (*task_ptr);
};

void swap(struct Qnode * a, struct Qnode * b);

int decrementAll(int counter);
