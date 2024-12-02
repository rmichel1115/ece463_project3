#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "sim_proc.h"

/*  argc holds the number of command line arguments
    argv[] holds the commands themselves

    Example:-
    sim 256 32 4 gcc_trace.txt
    argc = 5
    argv[0] = "sim"
    argv[1] = "256"
    argv[2] = "32"
    ... and so on
*/

//steps of the pipeline instructions 
typedef struct {
    long int INSTR;
    long int src1;
    long int src2;
    long int dest;
    long int FETCH;
    long int DECODE;
    long int REGREAD;
    long int RENAME;
    long int WRITEBACK;
    long int RETIRE;
    long int EXECUTE;
    long int ISSUE;
    long int DISPATCH;
    uint64_t pc;

} ProcessorInformation;

# define MAXIMUM 1000000
// double instructions_per_cycle = 0;
// uint64_t track_instructions = 0;
// uint64_t track_cycle = 0;
// long int finish_file = 0;
// PL processor_PL;

long int instruction_tracker = 0;
long int instruction_stop = 0;
long int PL_cycle = 0;
ProcessorInformation read_instruction[MAXIMUM];
//function below begins the pipeline 

// void PL_Start(proc_params params){
//     processor_PL.width = params.width;
//     processor_PL.iq_size = params.iq_size;
//     processor_PL.rob_size = params.rob_size; 
//     processor_PL.PL_cycle = 0;
//     processor_PL.CO_read = malloc(params.rob_size * sizeof(PL_instructions));
//     processor_PL.EX_read = malloc(params.width * sizeof(PL_instructions));
//     processor_PL.IS_read = malloc(params.rob_size * sizeof(PL_instructions));
//     processor_PL.FE_read = malloc(params.width * sizeof(PL_instructions));
// }

//defining functions for pipeline simulation (reverse order)

// void Commit(){
//     printf("Instructions Committed\n");
// }

void Writeback() {
    if (instruction_tracker > 6)
    read_instruction[instruction_stop].WRITEBACK = PL_cycle;
    }

void Execute() {
    if (instruction_tracker > 5)
    read_instruction[instruction_stop].EXECUTE = PL_cycle;
    }


void Issue() {
    if (instruction_tracker > 4)
    read_instruction[instruction_stop].ISSUE = PL_cycle;
    }

void Decode(){
    if (instruction_tracker > 0 )
    read_instruction[instruction_stop].DECODE = PL_cycle;


}

void Fetch(FILE *FP, proc_params params){
if (fscanf(FP, "%lx %d %d %d %d", &read_instruction[instruction_tracker].pc, &read_instruction[instruction_tracker].INSTR, &read_instruction[instruction_tracker].dest, &read_instruction[instruction_tracker].src1, &read_instruction[instruction_tracker].src2) != EOF){
    instruction_tracker++;
    read_instruction[instruction_tracker].FETCH = PL_cycle;
}

}


void Dispatch() {
    if (instruction_tracker > 3)
    read_instruction[instruction_stop].DISPATCH = PL_cycle;
    }

void RegRead() {
    if (instruction_tracker > 2)
    read_instruction[instruction_stop].REGREAD = PL_cycle;
    }


void Rename() {
    if (instruction_tracker > 1)
    read_instruction[instruction_stop].RENAME = PL_cycle;
    }

void Retire() {
    if (instruction_tracker > instruction_stop) {
        read_instruction[instruction_stop].RETIRE = PL_cycle;
        instruction_stop++;
    }
}
 //recall pipeline stages in reverse order 
int Advance_Cycle() {

 PL_cycle++;
 Retire();
 Writeback();
 Execute();
 Issue();
 Dispatch();
 RegRead();
 Rename(); 
 Decode();
 Fetch(NULL, (proc_params){0});
 return instruction_stop < instruction_tracker;
}

int main (int argc, char* argv[])
{
    FILE *FP;               // File handler
    char *trace_file;       // Variable that holds trace file name;
    proc_params params;       // look at sim_bp.h header file for the the definition of struct proc_params
    int op_type, dest, src1, src2;  // Variables are read from trace file
    uint64_t pc; // Variable holds the pc read from input file
    
    if (argc != 5)
    {
        printf("Error: Wrong number of inputs:%d\n", argc-1);
        exit(EXIT_FAILURE);
    }
    
    params.rob_size     = strtoul(argv[1], NULL, 10);
    params.iq_size      = strtoul(argv[2], NULL, 10);
    params.width        = strtoul(argv[3], NULL, 10);
    trace_file          = argv[4];
    printf("rob_size:%lu "
            "iq_size:%lu "
            "width:%lu "
            "tracefile:%s\n", params.rob_size, params.iq_size, params.width, trace_file);
    // Open trace_file in read mode
    FP = fopen(trace_file, "r");
    if(FP == NULL)
    {
        // Throw error and exit if fopen() failed
        printf("Error: Unable to open file %s\n", trace_file);
        exit(EXIT_FAILURE);
    }
      printf("=== Simulator Command ======\n# ./sim %lu %lu %lu %s\n", params.rob_size, params.iq_size, params.width, argv[4]);
      printf("=== Processor Configuration ===\n# ROB_SIZE = %lu\n# IQ_SIZE = %lu\n# WIDTH = %lu\n", params.rob_size, params.iq_size, params.width);
  
   
    for (int j = 0; j < instruction_tracker; j++) {
        printf("%d fu{%d} src{%d,%d} dst{%d} FE{%d,1} DE{%d,1} RN{%d,1} RR{%d,1} DI{%d,1} IS{%d,1} EX{%d,1} WB{%d,1} RT{%d,1}\n", j, read_instruction[j].INSTR, read_instruction[j].src1, read_instruction[j].src2, read_instruction[j].dest, read_instruction[j].FETCH, read_instruction[j].DECODE, read_instruction[j].RENAME, read_instruction[j].REGREAD, read_instruction[j].DISPATCH, read_instruction[j].ISSUE, read_instruction[j].EXECUTE, read_instruction[j].WRITEBACK, read_instruction[j].RETIRE);
    }
   do {
    Retire();
    Writeback();
    Execute();
    Issue();
    Dispatch();
    RegRead();
    Rename();
    Decode();
    Fetch(FP, params);

   } while (Advance_Cycle());

    printf("=== Simulation Results =======\n");
    printf(" Dynamic Instruction Count = %d\n", instruction_tracker);
    printf(" Cycles = %d\n", PL_cycle);
    printf(" Instructions Per Cycle (IPC) = %.2f\n", instruction_tracker / PL_cycle);   
  
    
    //while(fscanf(FP, "%lx %d %d %d %d", &pc, &op_type, &dest, &src1, &src2) != EOF)
        //printf("%lx %d %d %d %d\n", pc, op_type, dest, src1, src2); //Print to check if inputs have been read correctly
    fclose(FP);
    return 0;
}

