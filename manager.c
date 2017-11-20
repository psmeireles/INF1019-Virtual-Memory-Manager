#include<stdio.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/stat.h>
#include<sys/shm.h>

int main(){
    pid_t pidp1, pidp2, pidp3, pidp4;

    if((pidp1 = fork()) == 0){ // User Process 1
        FILE *simulador;
        
        simulador = fopen("Logs/simulador.log", "r");
        if(simulador == NULL){
            printf("Error when opening file simulador.log\n");
            exit(1);
        }
        
    }
    else if((pidp2 = fork()) == 0){ // User Process 2
        FILE *matriz;
        
        matriz = fopen("Logs/matriz.log", "r");
        if(matriz == NULL){
            printf("Error when opening file matriz.log\n");
            exit(1);
        }
    }
    else if((pidp3 = fork()) == 0){ // User Process 3
        FILE *compressor;
        
        compressor = fopen("Logs/compressor.log", "r");
        if(compressor == NULL){
            printf("Error when opening file compressor.log\n");
            exit(1);
        }
    }
    else if((pidp4 = fork()) == 0){ // User Process 4
        FILE *compilador;
        
        compilador = fopen("Logs/compilador.log", "r");
        if(compilador == NULL){
            printf("Error when opening file compilador.log\n");
            exit(1);
        }
    }
    else{

    }
}