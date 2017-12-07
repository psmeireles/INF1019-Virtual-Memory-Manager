/*
PUC-Rio  - Trabalho de Sistemas de Computação (INF1019)
Professor: Markus Endler
Monitora: Jéssica Almeida
Alunos:
Pedro Sousa Meireles -  1510962
Julio Neuman Kessel  -  1511745
*/

#include <sys/sem.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "semaphore.h"

union semun
{
	int val;
	struct semid_ds *buf;
	ushort *array;
};


int setSemValue(int semId)
{
	union semun semUnion;
	semUnion.val = 1;
	return semctl(semId, 0, SETVAL, semUnion);
}

void delSemValue(int semId)
{
	union semun semUnion;
	semctl(semId, 0, IPC_RMID, semUnion);
}

int down(int semId)
{
	struct sembuf semB;
	semB.sem_num = 0;
	semB.sem_op = -1;
	semB.sem_flg = SEM_UNDO;
	semop(semId, &semB, 1);
	return 0;
}

int up(int semId)
{
	struct sembuf semB;
	semB.sem_num = 0;
	semB.sem_op = 1;
	semB.sem_flg = SEM_UNDO;
	semop(semId, &semB, 1);
	return 0;
}
