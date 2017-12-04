#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<signal.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<sys/stat.h>
#include<sys/shm.h>
#include<time.h>
#include"definitions.h"
#include"vm.h"

#define EVER ;;

int LFU();
void pageFaultHandler(int signal);
void quitHandler(int sinal);
void childHanlder(int sinal);

PageTableElement *table[4];
PageFrame *pf;
pid_t pidp[4];

QueueVector *qv;

int activeProcesses = 4;
int swapW = 0;
int pageFaults = 0;

time_t now;

int main(){
    int i;
    now = time(0);

	pf = createPageFrames();

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
        
        simulador = fopen("Logs/simulador.log", "r");
        if(simulador == NULL){
            printf("Error when opening file simulador.log\n");
            exit(1);
        }

        while(fscanf(simulador, "%x %c", &addr, &rw) > 0){
            int i = addr >> 16, o = addr - (i << 16);
            table[i].page.offset = o;
            table[i].page.type = rw;

            trans(0, i, o, rw);
        }
        
    }
    else if((pidp[1] = fork()) == 0){ // User Process 2
        FILE *matriz;
        unsigned addr;
        char rw;
        PageTableElement *table;

        table = getPageTable(1);
        
        matriz = fopen("Logs/matriz.log", "r");
        if(matriz == NULL){
            printf("Error when opening file matriz.log\n");
            exit(1);
        }

        while(fscanf(matriz, "%x %c", &addr, &rw) > 0){
            int i = addr >> 16, o = addr - (i << 16);
            table[i].page.offset = o;
            table[i].page.type = rw;

            trans(1, i, o, rw);
        }
    }
    else if((pidp[2] = fork()) == 0){ // User Process 3
        FILE *compressor;
        unsigned addr;
        char rw;
        PageTableElement *table;

        table = getPageTable(2);
        
        compressor = fopen("Logs/compressor.log", "r");
        if(compressor == NULL){
            printf("Error when opening file compressor.log\n");
            exit(1);
        }

        while(fscanf(compressor, "%x %c", &addr, &rw) > 0){
            int i = addr >> 16, o = addr - (i << 16);
            table[i].page.offset = o;
            table[i].page.type = rw;

            trans(2, i, o, rw);
        }
    }
    else if((pidp[3] = fork()) == 0){ // User Process 4
        FILE *compilador;
        unsigned addr;
        char rw;
        PageTableElement *table;

        table = getPageTable(3);
        
        compilador = fopen("Logs/compilador.log", "r");
        if(compilador == NULL){
            printf("Error when opening file compilador.log\n");
            exit(1);
        }
  
        while(fscanf(compilador, "%x %c", &addr, &rw) > 0){
            int i = addr >> 16, o = addr - (i << 16);
            table[i].page.offset = o;
            table[i].page.type = rw;

            trans(3, i, o, rw);
        }
    }
    else{   // Memory Manager
        signal(SIGUSR1, pageFaultHandler);
        signal(SIGQUIT, quitHandler);
        signal(SIGINT, quitHandler);
        signal(SIGCHLD, childHanlder);

        
        while(activeProcesses > 0){
        	sleep(1);
        }

        now = time(0) - now;
        printf("Time: %lu\nPage Faults: %d\nSwap W: %d\n", now, pageFaults, swapW);

        return 0;
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
	kill(pidp[pg.proc_number], SIGSTOP);
    pageFaults++;
	qv->first = (qv->first + 1) % 4;    
    newFrameIndex = LFU();
    table[pf[newFrameIndex].page.proc_number][pf[newFrameIndex].page.index].frame.count = -1;
    table[pf[newFrameIndex].page.proc_number][pf[newFrameIndex].page.index].frame.index = -1;
	pf[newFrameIndex].count = 1;
	pf[newFrameIndex].page.index = pg.index;
	pf[newFrameIndex].page.proc_number = pg.proc_number;
	pf[newFrameIndex].page.offset = pg.offset;
	pf[newFrameIndex].page.type = pg.type;
    if(pf[newFrameIndex].page.type == 'W'){
        swapW++;
    }
    table[pg.proc_number][pg.index].frame.count = pf[newFrameIndex].count;
    table[pg.proc_number][pg.index].frame.index = pf[newFrameIndex].index;
    table[pg.proc_number][pg.index].frame.page.index = pf[newFrameIndex].page.index;
    table[pg.proc_number][pg.index].frame.page.proc_number = pf[newFrameIndex].page.proc_number;
    table[pg.proc_number][pg.index].frame.page.offset = pf[newFrameIndex].page.offset;
    table[pg.proc_number][pg.index].frame.page.type = pf[newFrameIndex].page.type;
    kill(pidp[pg.proc_number], SIGCONT);
}

void quitHandler(int sinal){
    int i;
    for(i = 0; i < 256; i++){
        printf("Frame %d: %d\n", i, pf[i].count);
    }

    now = time(0) - now;
    printf("Time: %lu\nPage Faults: %d\nSwap W: %d\n", now, pageFaults, swapW);

	clearShm();
    printf("\nTerminando...\n");
    exit (0);
}

void childHanlder(int sinal){
    int pid_encerrado = waitpid(-1, NULL, WNOHANG);
    if(pid_encerrado == 0){ // Ainda não acabou
        return;
    }
    activeProcesses--;
}
