typedef struct page{
    int index;
    int proc_number;
    char bitM; // 1 for modified. Else 0
} Page;

typedef struct pageFrame{
    int count; 
    int index;
    Page page;
} PageFrame;

typedef struct pageTableElement{
    Page page;
    PageFrame frame;
} PageTableElement;

typedef struct queueVector{
    Page pages[4];
    int first;
    int empty;
} QueueVector;
