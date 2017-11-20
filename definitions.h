typedef struct PageFrame{
    int count;
    Page a;
} PageFrame;

typedef struct Page{
    int index;
    // The atributes below refer to the current access to the page
    int proc_number;
    int offset;
    char type; // 'r' or 'w'
} Page;

typedef struct PageTableElement{
    Page page;
    PageFrame frame;
} PageTableElement;