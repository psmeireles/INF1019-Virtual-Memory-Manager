#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<signal.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<sys/stat.h>
#include<sys/shm.h>
#include"definitions.h"
#include"vm.h"

#define EVER ;;

int LFU();
void pageFaultHandler(int signal);
void quitHandler(int sinal);

PageTableElement *table[4];
PageFrame pf[256];
pid_t pidp[4];

QueueVector *qv;

int main(){
    int i;

	createPageFrames(pf);

	// Initializing Page Tables
    for(i = 0; i < 4; i++){
        table[i] = createPageTable(i);
    }
    
    qv = createQueueVector();

    if((pidp[0] = fork()) == 0){ // User Process 1
        FILE *simulador;
        unsigned addr;
        char rw;
        PageTableElement *table;
        
        table = getPageTable(0);
		//printf("\nEnter process 1\n");
        
        simulador = fopen("Logs/simulador.log", "r");
        if(simulador == NULL){
            printf("Error when opening file simulador.log\n");
            exit(1);
        }

        while(fscanf(simulador, "%x %c", &addr, &rw) > 0){
            int i = addr >> 16, o = addr - (i << 16);
            table[i].page.offset = o;
            table[i].page.type = rw;

            //printf("\ntrans call process 1\n");
            trans(0, i, o, rw);
        }
        
    }
    else if((pidp[1] = fork()) == 0){ // User Process 2
        FILE *matriz;
        unsigned addr;
        char rw;
        PageTableElement *table;

        table = getPageTable(1);
        //printf("\nEnter process 2\n");
        
        matriz = fopen("Logs/matriz.log", "r");
        if(matriz == NULL){
            printf("Error when opening file matriz.log\n");
            exit(1);
        }

        while(fscanf(matriz, "%x %c", &addr, &rw) > 0){
            int i = addr >> 16, o = addr - (i << 16);
            table[i].page.offset = o;
            table[i].page.type = rw;

            //printf("\ntrans call process 2\n");
            trans(1, i, o, rw);
        }
    }
    else if((pidp[2] = fork()) == 0){ // User Process 3
        FILE *compressor;
        unsigned addr;
        char rw;
        PageTableElement *table;

        table = getPageTable(2);
        //printf("\nEnter process 3\n");
        
        compressor = fopen("Logs/compressor.log", "r");
        if(compressor == NULL){
            printf("Error when opening file compressor.log\n");
            exit(1);
        }

        while(fscanf(compressor, "%x %c", &addr, &rw) > 0){
            int i = addr >> 16, o = addr - (i << 16);
            table[i].page.offset = o;
            table[i].page.type = rw;

            //printf("\ntrans call process 3\n");
            trans(2, i, o, rw);
        }
    }
    else if((pidp[3] = fork()) == 0){ // User Process 4
        FILE *compilador;
        unsigned addr;
        char rw;
        PageTableElement *table;

        table = getPageTable(3);
        //printf("\nEnter process 4\n");
        
        compilador = fopen("Logs/compilador.log", "r");
        if(compilador == NULL){
            printf("Error when opening file compilador.log\n");
            exit(1);
        }
  
        while(fscanf(compilador, "%x %c", &addr, &rw) > 0){
            int i = addr >> 16, o = addr - (i << 16);
            table[i].page.offset = o;
            table[i].page.type = rw;

            //printf("\ntrans call process 4\n");
            trans(3, i, o, rw);
        }
    }
    else{   // Memory Manager
    	//printf("\nEnter memory manager\n");
        signal(SIGUSR1, pageFaultHandler);
        signal(SIGQUIT, quitHandler);
        signal(SIGINT, quitHandler);
        
        for(EVER){
        	sleep(1);
        }
    }
}

int LFU(){
    
    int i;
    int minCount, minFrame;

    minCount = pf[0].count;
    minFrame = 0;
    for( i = 1; i<256; i++ ){
        if(minCount == 0){
    		return minFrame;
    	}
        if(pf[i].count < minCount){
            minCount = pf[i].count;
            minFrame = i;
        }
    }

    return minFrame;
}

void pageFaultHandler(int signal){
    int newFrameIndex, loserProcess;
    Page pg;
	
	pg = getCurrentRequest();
	
	qv->first = (qv->first + 1) % 4;

	//printf("\nEnter Handler - %d\n", pg.proc_number+1);

	//printf("\nSigstop index %04x offset %04x\n", pg.index, pg.offset);
    kill(pidp[pg.proc_number], SIGSTOP);
    newFrameIndex = LFU();
    //printf("\nLFU\n");
    table[pf[newFrameIndex].page.proc_number][pf[newFrameIndex].page.index].frame.count = -1;
    table[pf[newFrameIndex].page.proc_number][pf[newFrameIndex].page.index].frame.index = -1;
    //printf("\nAtualizou page table \n");
	pf[newFrameIndex].count = 1;
	pf[newFrameIndex].page.index = pg.index;
	pf[newFrameIndex].page.proc_number = pg.proc_number;
	pf[newFrameIndex].page.offset = pg.offset;
	pf[newFrameIndex].page.type = pg.type;
	//printf("\nAtualizou Page frame\n");
    table[pg.proc_number][pg.index].frame.count = pf[newFrameIndex].count;
    table[pg.proc_number][pg.index].frame.index = pf[newFrameIndex].index;
    table[pg.proc_number][pg.index].frame.page.index = pf[newFrameIndex].page.index;
    table[pg.proc_number][pg.index].frame.page.proc_number = pf[newFrameIndex].page.proc_number;
    table[pg.proc_number][pg.index].frame.page.offset = pf[newFrameIndex].page.offset;
    table[pg.proc_number][pg.index].frame.page.type = pf[newFrameIndex].page.type;
    //printf("\nAlocado index %04x offset %04x na frame %04x\n", table[pg.proc_number][pg.index].frame.page.index, table[pg.proc_number][pg.index].frame.page.offset, table[pg.proc_number][pg.index].frame.index);
    kill(pidp[pg.proc_number], SIGCONT);
}

void quitHandler(int sinal){
	clearShm();
    printf("\nTerminando...\n");
    exit (0);
}




