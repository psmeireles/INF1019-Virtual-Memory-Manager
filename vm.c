#include"definitions.h"
#include"vm.h"
#include<sys/shm.h>
#include<sys/stat.h>
#include<math.h>

#define SIZE 65536

int seg[4];
PageTableElement *table[4];

PageTableElement* createPageTable(int pnumber){

    int i;
    seg[pnumber] = shmget(IPC_PRIVATE, SIZE*sizeof(PageTableElement), IPC_CREAT | IPC_EXCL | S_IRWXU);
    table[pnumber] = (PageTableElement *) shmat(seg[pnumber], 0, 0);

    // Initialize table
    for(i = 0; i < SIZE; i++){
        table[pnumber][i].frame = NULL;
        table[pnumber][i].page.index = i;
        table[pnumber][i].page.proc_number = pnumber;
        table[pnumber][i].page.offset = NULL;
        table[pnumber][i].page.type = NULL;
    }

    return table[pnumber];
}

PageTableElement* getPageTable(int pnumber){
    return table[pnumber];
}

void trans(int pnumber, unsigned int i, unsigned int o, char rw){

    Page* pg;

    int segmento;
    Page *pagina;

    segmento = shmget(2220, sizeof(Page), IPC_CREAT | IPC_EXCL | S_IRWXU);
    pagina = (Page*)shmat(segmento, 0, 0);

    pg->index = i;
    pg->proc_number = pnumber;
    pg->offset = o;
    pg->type = rw;

    *pagina = *pg;

    if(table[pnumber][i].frame != NULL){
        printf("P%d, %x, %c", pnumber, table[pnumber][i].frame.index*pow(2, 16)+o, rw);
    }
    else{
        kill(getppid(), SIGUSR1);   // Ask MM for Page Frame
        pause();                    // Waits until MM sets a Page Frame to current page
        printf("P%d, %x, %c", pnumber, table[pnumber][i].frame.index*pow(2, 16)+o, rw);
    }
}




