#ifndef SIM_PROC_H
#define SIM_PROC_H

typedef struct proc_params{
    unsigned long int rob_size;
    unsigned long int iq_size;
    unsigned long int width;
}proc_params;


//list of funcitons 

int Advance_Cycle(FILE *FP, proc_params parameters);
void Commit();
void Execute();
void Issue(proc_params parameters);
void Decode(proc_params parameters);
void Fetch(FILE *FP, proc_params parameters);
void PL_Start(proc_params parameters);

typedef struct PL_instructions {
    long int src1;
    long int src2;
    long int op_type;
    long int dest;
    uint64_t pc;

} PL_instructions;

typedef struct PL {

    unsigned long int width;
    unsigned long int iq_size;
    unsigned long int rob_size;
    unsigned long int PL_cycle;
    PL_instructions *CO_read;
    PL_instructions *EX_read;
    PL_instructions *IS_read;
    PL_instructions *FE_read;

} PL;

#endif

