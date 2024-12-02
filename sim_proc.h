#ifndef SIM_PROC_H
#define SIM_PROC_H

typedef struct proc_params{
    unsigned long int rob_size;
    unsigned long int iq_size;
    unsigned long int width;
}proc_params;


//list of funcitons 

int Advance_Cycle();
void Writeback();
void Retire();
void RegRead();
void Rename();
void Execute();
void Dispatch();
void Issue();
void Decode();
void Fetch(FILE *FP, proc_params params);

#endif
