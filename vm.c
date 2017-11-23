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
    
    printf("4\n");
    seg[pnumber] = shmget(IPC_PRIVATE, SIZE*sizeof(PageTableElement), IPC_CREAT | IPC_EXCL | S_IRWXU);	// Page Table
    printf("5\n");
    table[pnumber] = (PageTableElement *) shmat(seg[pnumber], 0, 0);
	printf("6\n");
        
    if(requests == NULL){
    	requests = queue_create();
    }
    
    printf("7\n");

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
    return table[pnumber];
}

PageTableElement* getPageTable(int pnumber){
    return table[pnumber];
}

void trans(int pnumber, unsigned int i, unsigned int o, char rw){

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
        kill(getppid(), SIGUSR1);   // Ask MM for Page Frame
        pause();                    // Waits until MM sets a Page Frame to current page
        printf("P%d, %x%x, %c", pnumber, table[pnumber][i].frame->index, o, rw);
    }
}

Page * getCurrentRequest(){
	return queue_pop(requests);	
}

void clearShm(){
	int i;
	
	for(i = 0; i < 4; i++){
	    shmctl(seg[i], IPC_RMID, 0);
    }
    queue_free(requests);
}


