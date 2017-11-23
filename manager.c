#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/stat.h>
#include<sys/shm.h>
#include"definitions.h"
#include"vm.h"

int LFU(PageFrame *pf);
void pageFaultHandler(int signal);
void quitHandler(int sinal);

PageTableElement *table[4];
PageFrame pf[256];

int main(){
    pid_t pidp[4];
    int i;

    for(i = 0; i < 4; i++){
        table[i] = createPageTable(i);
    }
    

    if((pidp[1] = fork()) == 0){ // User Process 1
        FILE *simulador;
        unsigned addr;
        char rw;
        PageTableElement *table;
        
        table = getPageTable(1);

        
        simulador = fopen("Logs/simulador.log", "r");
        if(simulador == NULL){
            printf("Error when opening file simulador.log\n");
            exit(1);
        }

        while(fscanf(simulador, "%x %c", &addr, &rw) > 0){
            unsigned int i = addr >> 16, o = addr - (i << 16);
            table[i].page.offset = o;
            table[i].page.type = rw;

            trans(1, i, o, rw);
        }
        
    }
    else if((pidp[2] = fork()) == 0){ // User Process 2
        FILE *matriz;
        unsigned addr;
        char rw;
        PageTableElement *table;
        
        table = getPageTable(2);
        
        matriz = fopen("Logs/matriz.log", "r");
        if(matriz == NULL){
            printf("Error when opening file matriz.log\n");
            exit(1);
        }

        while(fscanf(matriz, "%x %c", &addr, &rw) > 0){
            unsigned int i = addr >> 16, o = addr - (i << 16);
            table[i].page.offset = o;
            table[i].page.type = rw;

            trans(2, i, o, rw);
        }
    }
    else if((pidp[3] = fork()) == 0){ // User Process 3
        FILE *compressor;
        unsigned addr;
        char rw;
        PageTableElement *table;
        
        table = getPageTable(3);
        
        compressor = fopen("Logs/compressor.log", "r");
        if(compressor == NULL){
            printf("Error when opening file compressor.log\n");
            exit(1);
        }

        while(fscanf(compressor, "%x %c", &addr, &rw) > 0){
            unsigned int i = addr >> 16, o = addr - (i << 16);
            table[i].page.offset = o;
            table[i].page.type = rw;

            trans(3, i, o, rw);
        }
    }
    else if((pidp[4] = fork()) == 0){ // User Process 4
        FILE *compilador;
        unsigned addr;
        char rw;
        PageTableElement *table;
        
        table = getPageTable(4);
        
        compilador = fopen("Logs/compilador.log", "r");
        if(compilador == NULL){
            printf("Error when opening file compilador.log\n");
            exit(1);
        }

        while(fscanf(compilador, "%x %c", &addr, &rw) > 0){
            unsigned int i = addr >> 16, o = addr - (i << 16);
            table[i].page.offset = o;
            table[i].page.type = rw;

            trans(4, i, o, rw);
        }
    }
    else{   // Memory Manager

        signal(SIGUSR1, pageFaultHandler);

        // Initializing Page Frames
        for(i = 0; i < 256; i++){
            pf[i].count = 0;
            pf[i].index = i;
            pf[i].page = NULL;
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
    }

    return minFrame;
}

void pageFaultHandler(int signal){

    int segmento, newFrameIndex, loserProcess;
    Page *pg, *newPage = (Page *) malloc (sizeof(Page));

    segmento = shmget(2220, sizeof(Page), IPC_CREAT | S_IRWXU);
    pg = (Page*)shmat(segmento, 0, 0);

    newPage->index = pg->index;
    newPage->proc_number = pg->proc_number;
    newPage->offset = pg->offset;
    newPage->type = pg->type;

    kill(pidp[newPage->proc_number] ,SIGSTOP);

    newFrameIndex = LFU();

    loserProcess = pf[newFrameIndex].page.proc_number;

    pf[newFrameIndex].count = pf[newFrameIndex].count + 1;
    pf[newFrameIndex].page = newPage;

    table[newPage->proc_number][newPage->index].frame = pf[newFrameIndex];

    kill(pidp[loserProcess], SIGUSR2);

    kill(pidp[newPage->proc_number] ,SIGCONT);
}

void quitHandler(int sinal)
{
    int segmento;
    int i;

    segmento = shmget(2220, 0, S_IRWXU);
    shmctl(segmento, IPC_RMID, 0);


    printf("\nTerminando...\n");
    exit (0);
}




