#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<sys/shm.h>
#include<sys/stat.h>
#include<unistd.h>
#include"queue.h"
#include"definitions.h"
#include"vm.h"

#define SIZE 65536

int seg[4];
PageTableElement *table[4];
Queue *requests;
PageFrame pf[256];

PageFrame* createPageFrames(){

	int i;
	// Initializing Page Frames
	for(i = 0; i < 256; i++){
		pf[i].count = 0;
		pf[i].index = i;
		pf[i].page = NULL;	
	} 
	
	return pf;
}

PageTableElement* createPageTable(int pnumber){

    int i;
    
    seg[pnumber] = shmget(IPC_PRIVATE, SIZE*sizeof(PageTableElement), IPC_CREAT | IPC_EXCL | S_IRWXU);	// Page Table
    table[pnumber] = (PageTableElement *) shmat(seg[pnumber], 0, 0);
      
    if(requests == NULL){
    	requests = queue_create();
    }
    
    table[pnumber] = (PageTableElement *) malloc(65536*sizeof(PageTableElement));

    // Initialize table
    for(i = 0; i < SIZE; i++){

    	Page *pg = (Page *) malloc(sizeof(Page));
             
		pg->index = i;
        pg->proc_number = pnumber;
        pg->offset = NULL;
        pg->type = NULL;
        pg->bitM = 0;
        
        table[pnumber][i].frame = NULL;
        table[pnumber][i].page = pg; 
    }
    printf("\nEnd createPageTable process %d\n", pnumber+1);
    return table[pnumber];
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
        Page *pg = (Page *) malloc(sizeof(Page));
        
	    pg->index = i;
	    pg->proc_number = pnumber;
	    pg->offset = o;
	    pg->type = rw;
	    
    	queue_push(requests, pg);
        printf("\nsend SIGUSR1 process %d\n", pnumber+1);	
        kill(getppid(), SIGUSR1);   // Ask MM for Page Frame
        pause();                    // Waits until MM sets a Page Frame to current page
        printf("P%d, %x%x, %c", pnumber, table[pnumber][i].frame->index, o, rw);
    }
}

Page * getCurrentRequest(){
    printf("\nEnter getCurrentRequest\n");
	return queue_pop(requests);
}

void clearShm(){
	int i;
	
	for(i = 0; i < 4; i++){
	    shmctl(seg[i], IPC_RMID, 0);
    }
    queue_free(requests);
}


