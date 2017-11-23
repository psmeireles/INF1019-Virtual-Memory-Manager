typedef struct queue Queue;
typedef struct node Node;

Queue* queue_create ();
void queue_push (Queue* q, void* obj);
void * queue_pop (Queue* q);
int queue_isempty (Queue* q);
void queue_free (Queue* q); 
