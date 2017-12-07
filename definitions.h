/*
PUC-Rio  - Trabalho de Sistemas de Computação (INF1019)
Professor: Markus Endler
Monitora: Jéssica Almeida
Alunos:
Pedro Sousa Meireles -  1510962
Julio Neuman Kessel  -  1511745
*/

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
