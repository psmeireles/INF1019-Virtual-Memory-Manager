# Virtual memomry manager

Assignment for INF1019 - Computer Systems in Pontifical Catholic University of Rio de Janeiro (PUC-Rio)

## Group members

[Julio Neuman Kessel](https://github.com/JNKessel)

[Pedro Sousa Meireles](https://github.com/psmeireles)

## Usage

Open a terminal and run `gcc -o manager manager.c vm.c semaphore.c` and then `./manager`.

## How it works

The vm.c and semaphore.c files work simply as libraries that will be used by the manager.

As soon as program execution starts, the page frames array shared by the 4 processes
used by the program will be initialized, and are also initialized 4 page tables, one for each process.

Each process is loops reading line by line the files containing the addresses of
virtual memory accessed by each process, and at each iteration they make a call
for the trans function of vm.c by passing the process number, page index, offset and type of access.

The trans function then verifies whether this page is already related to a page frame.
If it already is, the counter of access numbers to the frame of this page is increased.
If not, a signal is sent back to the memory manager so that it uses the
LFU algorithm to decide on which page frame this page will be added to, and then update that 
information in both the page frames vector and the page table.
