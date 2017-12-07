/*
PUC-Rio  - Trabalho de Sistemas de Computação (INF1019)
Professor: Markus Endler
Monitora: Jéssica Almeida
Alunos:
Pedro Sousa Meireles -  1510962
Julio Neuman Kessel  -  1511745
*/

// Initialize semaphore value
int setSemValue(int semId);
// Removes semaphore
void delSemValue(int semId);

int down(int semId);
int up(int semId);
