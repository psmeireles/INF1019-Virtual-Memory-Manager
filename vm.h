void createPageFrames(PageFrame *pf);
QueueVector* createQueueVector();
PageTableElement* createPageTable(int pnumber);
PageTableElement* getPageTable(int pnumber);
void trans(int pnumber, int i, unsigned int o, char rw);
Page getCurrentRequest();
void clearShm();
