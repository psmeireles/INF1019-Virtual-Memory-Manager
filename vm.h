PageFrame * createPageFrames();
PageTableElement* createPageTable(int pnumber);
PageTableElement* getPageTable(int pnumber);
void trans(int pnumber, unsigned int i, unsigned int o, char rw);
Page * getCurrentRequest();
void clearShm();
