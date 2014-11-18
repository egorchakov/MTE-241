typedef struct {
    array_type *array;
    size_t length;
} array_t;

typedef struct {
    array_t array;
    size_t a;
    size_t c;
} array_interval_t;

typedef struct{
    array_interval_t interval;
    unsigned char priority;
} qsort_task_parameters_t;