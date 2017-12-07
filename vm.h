/*
PUC-Rio  - Trabalho de Sistemas de Computação (INF1019)
Professor: Markus Endler
Monitora: Jéssica Almeida
Alunos:
Pedro Sousa Meireles -  1510962
Julio Neuman Kessel  -  1511745
*/

PageFrame * createPageFrames();
QueueVector* createQueueVector();
PageTableElement* createPageTable(int pnumber);
PageTableElement* getPageTable(int pnumber);
void trans(int pnumber, int i, unsigned int o, char rw);
Page getCurrentRequest();
void clearShm();
