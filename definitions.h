typedef struct page{
    int index;
    // The atributes below refer to the current access to the page
    int proc_number;
    int offset;
    char type; // 'r' or 'w'
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
