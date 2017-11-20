typedef struct PageFrame{
    int index;
    int count;
    Access a;
} PageFrame;

typedef struct Access{
    int proc_number;
    int page_index;
    int offset;
    char type; // 'r' or 'w'
} Access;