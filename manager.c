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
PageFrame *pf;
pid_t pidp[4];

int main(){
    int i;

	pf = createPageFrames();

	for(i = 0; i < 256; i++)
		printf("%d\n", pf[i].index);

	// Initializing Page Tables
    for(i = 0; i < 4; i++){
        table[i] = createPageTable(i);
    }

    if((pidp[0] = fork()) == 0){ // User Process 1
        FILE *simulador;
        unsigned addr;
        char rw;
        PageTableElement *table;
        sleep(3);
        table = getPageTable(0);
		printf("\nEnter process 1\n");
        
        simulador = fopen("Logs/simulador.log", "r");
        if(simulador == NULL){
            printf("Error when opening file simulador.log\n");
            exit(1);
        }

        while(fscanf(simulador, "%x %c", &addr, &rw) > 0){
            unsigned int i = addr >> 16, o = addr - (i << 16);
            table[i].page->offset = o;
            table[i].page->type = rw;

            printf("\ntrans call process 1\n");
            trans(0, i, o, rw);
        }
        
    }
    else if((pidp[1] = fork()) == 0){ // User Process 2
        FILE *matriz;
        unsigned addr;
        char rw;
        PageTableElement *table;
        sleep(3);
        table = getPageTable(1);
        printf("\nEnter process 2\n");
        
        matriz = fopen("Logs/matriz.log", "r");
        if(matriz == NULL){
            printf("Error when opening file matriz.log\n");
            exit(1);
        }

        while(fscanf(matriz, "%x %c", &addr, &rw) > 0){
            unsigned int i = addr >> 16, o = addr - (i << 16);
            table[i].page->offset = o;
            table[i].page->type = rw;

            printf("\ntrans call process 2\n");
            trans(1, i, o, rw);
        }
    }
    else if((pidp[2] = fork()) == 0){ // User Process 3
        FILE *compressor;
        unsigned addr;
        char rw;
        PageTableElement *table;
        sleep(3);
        table = getPageTable(2);
        printf("\nEnter process 3\n");
        
        compressor = fopen("Logs/compressor.log", "r");
        if(compressor == NULL){
            printf("Error when opening file compressor.log\n");
            exit(1);
        }

        while(fscanf(compressor, "%x %c", &addr, &rw) > 0){
            unsigned int i = addr >> 16, o = addr - (i << 16);
            table[i].page->offset = o;
            table[i].page->type = rw;

            printf("\ntrans call process 3\n");
            trans(2, i, o, rw);
        }
    }
    else if((pidp[3] = fork()) == 0){ // User Process 4
        FILE *compilador;
        unsigned addr;
        char rw;
        PageTableElement *table;

        sleep(3);
        
        table = getPageTable(3);
        printf("\nEnter process 4\n");
        
        compilador = fopen("Logs/compilador.log", "r");
        if(compilador == NULL){
            printf("Error when opening file compilador.log\n");
            exit(1);
        }
  
        while(fscanf(compilador, "%x %c", &addr, &rw) > 0){
            unsigned int i = addr >> 16, o = addr - (i << 16);
            table[i].page->offset = o;
            table[i].page->type = rw;

            printf("\ntrans call process 4\n");
            trans(3, i, o, rw);
        }
    }
    else{   // Memory Manager
    	printf("\nEnter memory manager\n");
        signal(SIGUSR1, pageFaultHandler);
        
        for(EVER){
        	pause();
        }
    }
}

int LFU(){
    
    int i;
    int minCount, minFrame;

    minCount = pf[0].count;
    minFrame = 0;
    for( i = 1; i<256; i++ ){

        if(pf[i].count < minCount){
            minCount = pf[i].count;
            minFrame = i;
        }
        else if(pf[i].count == minCount){
        	if(pf[i].page->bitM < pf[minFrame].page->bitM){
        		minFrame = i;
        	}
        }
    }

    return minFrame;
}

void pageFaultHandler(int signal){

	printf("\naaaaacccccc\n");

    int newFrameIndex, loserProcess;
    Page *pg;
	
	pg = getCurrentRequest(); //Possivel local de erro!!!!!!!!!!!

	printf("\nEnter Handler - %d\n", pg->proc_number);

    kill(pidp[pg->proc_number], SIGSTOP);

    newFrameIndex = LFU();
	
    loserProcess = pf[newFrameIndex].page->proc_number;

	pf[newFrameIndex].page->bitM = 1;
	
	pf[newFrameIndex].count = 1;
	pf[newFrameIndex].page = pg;

    table[pg->proc_number][pg->index].frame = &pf[newFrameIndex];

    kill(pidp[loserProcess], SIGUSR2);

    kill(pidp[pg->proc_number] ,SIGCONT);
}

void quitHandler(int sinal){
	clearShm();
    printf("\nTerminando...\n");
    exit (0);
}




