typedef struct page{
    int index;
    // The atributes below refer to the current access to the page
    int proc_number;
    int offset;
    char type; // 'r' or 'w'
    int bitM;
} Page;

typedef struct pageFrame{
    int count;
    int index;
    Page *a;
} PageFrame;

typedef struct pageTableElement{
    Page *page;
    PageFrame *frame;
} PageTableElement;
