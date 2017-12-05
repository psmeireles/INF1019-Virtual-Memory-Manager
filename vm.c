#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<sys/shm.h>
#include<sys/stat.h>
#include<sys/sem.h>
#include<unistd.h>
#include"definitions.h"
#include"vm.h"
#include"semaphore.h"

#define SIZE 65536


int segpf;
int seg[4];
int segQueue;
int semaphore;
PageTableElement *table[4];
PageFrame *pf;
QueueVector *qv;


PageFrame * createPageFrames(){

	int i;

    segpf = shmget(IPC_PRIVATE, 256*sizeof(PageFrame), IPC_CREAT | IPC_EXCL | S_IRWXU);	// Page Table
    pf = (PageFrame *) shmat(segpf, 0, 0);
	// Initializing Page Frames
	for(i = 0; i < 256; i++){
		pf[i].count = 0;
		pf[i].index = i;
	} 
    return pf;
}

PageTableElement* createPageTable(int pnumber){

    int i;
    
    seg[pnumber] = shmget(IPC_PRIVATE, SIZE*sizeof(PageTableElement), IPC_CREAT | IPC_EXCL | S_IRWXU);	// Page Table
    table[pnumber] = (PageTableElement *) shmat(seg[pnumber], 0, 0);

    // Initialize table
    for(i = 0; i < SIZE; i++){        
        table[pnumber][i].page.index = i;
        table[pnumber][i].page.proc_number = pnumber;
        table[pnumber][i].frame.count = -1; 
        table[pnumber][i].frame.index = -1;
    }
    return table[pnumber];
}

QueueVector* createQueueVector(){

	int i;

	segQueue = shmget(IPC_PRIVATE, sizeof(QueueVector), IPC_CREAT | IPC_EXCL | S_IRWXU);	// QueueVector
	
	semaphore = semget(IPC_PRIVATE, 1, 0666 | IPC_CREAT);
	setSemValue(semaphore);	// Initialize with 1
	
    qv = (QueueVector *) shmat(segQueue, 0, 0);
    
    qv->first = 0;
    qv->empty = 0;
    
    return qv;
}

PageTableElement* getPageTable(int pnumber){
    return table[pnumber];
}

void trans(int pnumber, int i, unsigned int o, char rw){
    
    int frameindex = table[pnumber][i].frame.index;
    int count = table[pnumber][i].frame.count;

    if( frameindex >= 0 && count >= 0){
        table[pnumber][i].frame.count++;
        pf[table[pnumber][i].frame.index].count++;

        if(rw == 'W'){
            table[pnumber][i].page.bitM = 1;
        }
        
        printf("P%d, %04x%04x, %c, count %d\n", pnumber + 1, frameindex, o, rw, table[pnumber][i].frame.count); 
    }
    else{
    
    	
    	down(semaphore);

    	qv->pages[qv->empty].index = i;
    	qv->pages[qv->empty].proc_number = pnumber;
    	qv->pages[qv->empty].offset = o;
    	qv->pages[qv->empty].type = rw;
        if(rw == 'W'){
            qv->pages[qv->empty].bitM = 1;
        }
        else{
            qv->pages[qv->empty].bitM = 0;
        }
    	
    	//update value that determines the next empty place on the vector
		qv->empty = (qv->empty + 1) % 4;
		
        kill(getppid(), SIGUSR1);   // Ask MM for Page Frame
        usleep(100000);                    // Waits until MM sets a Page Frame to current page
        //sleep(1);
        
        printf("P%d, %04x%04x, %c, count %d\n", pnumber + 1, table[pnumber][i].frame.index, o, rw, table[pnumber][i].frame.count); 
        up(semaphore);
    }

}

Page getCurrentRequest(){
	return qv->pages[qv->first];
}

void clearShm(){
	int i;
	
	for(i = 0; i < 4; i++){
	    shmctl(seg[i], IPC_RMID, 0);
    }
    shmctl(segQueue, IPC_RMID, 0);
    shmctl(segpf, IPC_RMID, 0);
}

