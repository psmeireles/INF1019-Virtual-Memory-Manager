#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<sys/shm.h>
#include<sys/stat.h>
#include<unistd.h>
#include"definitions.h"
#include"vm.h"

#define SIZE 65536

int seg[4];
int segQueue;
PageTableElement *table[4];
PageFrame pf[256];
QueueVector *qv;

void createPageFrames(PageFrame *pf){

	int i;
	// Initializing Page Frames
	for(i = 0; i < 256; i++){
		pf[i].count = 0;
		//printf("%d\n", pf[i].count);
		pf[i].index = i;
	} 
	return pf;
}

PageTableElement* createPageTable(int pnumber){

    int i;
    
    seg[pnumber] = shmget(IPC_PRIVATE, SIZE*sizeof(PageTableElement), IPC_CREAT | IPC_EXCL | S_IRWXU);	// Page Table
    table[pnumber] = (PageTableElement *) shmat(seg[pnumber], 0, 0);
      
    table[pnumber] = (PageTableElement *) malloc(65536*sizeof(PageTableElement));

    // Initialize table
    for(i = 0; i < SIZE; i++){

    	Page *pg = (Page *) malloc(sizeof(Page));
             
		pg->index = i;
        pg->proc_number = pnumber;
        pg->offset = NULL;
        pg->type = NULL;
        
        table[pnumber][i].frame = NULL;
        table[pnumber][i].page = pg; 
    }
    printf("\nEnd createPageTable process %d\n", pnumber+1);
    return table[pnumber];
}

QueueVector* createQueueVector(){

	int i;

	segQueue = shmget(IPC_PRIVATE, sizeof(QueueVector), IPC_CREAT | IPC_EXCL | S_IRWXU);	// QueueVector
    qv = (QueueVector *) shmat(segQueue, 0, 0);
    
    qv->first = 0;
    qv->empty = 0;
    
    return qv;
}

PageTableElement* getPageTable(int pnumber){
    return table[pnumber];
}

void trans(int pnumber, unsigned int i, unsigned int o, char rw){

    printf("\nEnter trans process %d\n", pnumber+1);

    if(table[pnumber][i].frame != NULL){
        printf("P%d, %x%x, %c", pnumber, table[pnumber][i].frame->index, o, rw);
		table[pnumber][i].frame->count++;
    }
    else{
//        Page *pg = (Page *) malloc(sizeof(Page));
        
	    //pg->index = i;
//	    pg->proc_number = pnumber;
//	    pg->offset = o;
//	    pg->type = rw;
	    
    	qv->pages[qv->empty].index = i;
    	qv->pages[qv->empty].proc_number = pnumber;
    	qv->pages[qv->empty].offset = o;
    	qv->pages[qv->empty].type = rw;
    	
		qv->empty = (qv->empty + 1) % 4;
        printf("\nsend SIGUSR1 process %d\n", pnumber+1);	
        kill(getppid(), SIGUSR1);   // Ask MM for Page Frame
        pause();                    // Waits until MM sets a Page Frame to current page
        printf("P%d, %x%x, %c", pnumber, table[pnumber][i].frame->index, o, rw);
    }
}

Page * getCurrentRequest(){
    printf("\nEnter getCurrentRequest\n");
	return &qv->pages[qv->first];
}

void clearShm(){
	int i;
	
	for(i = 0; i < 4; i++){
	    shmctl(seg[i], IPC_RMID, 0);
    }
    shmctl(segQueue, IPC_RMID, 0);
}


