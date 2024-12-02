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
 //this sets the maximum number of steps the pipeline can hit
# define MAXIMUM 1000000

//variables used throuhgout code 
long int instruction_tracker = 0;
long int instruction_stop = 0;
long int PL_cycle = 0;
ProcessorInformation read_instruction[MAXIMUM];

//functions that are within the pipeline
void Writeback() { //Writeback function
    if (instruction_tracker > 6)
    read_instruction[instruction_stop].WRITEBACK = PL_cycle; //sets the instruction to the pipeline cycle 
    }

void Execute() { //Execute function 
    if (instruction_tracker > 5)
    read_instruction[instruction_stop].EXECUTE = PL_cycle; //sets the instruction to the pipeline cycle 
    }

//Issue function 
void Issue() {
    if (instruction_tracker > 4)
    read_instruction[instruction_stop].ISSUE = PL_cycle; //sets the instruction to the pipeline cycle 
    }

void Decode(){ //Decode function 
    if (instruction_tracker > 0 )
    read_instruction[instruction_stop].DECODE = PL_cycle; //sets the instruction to the pipeline cycle 


}

void Fetch(FILE *FP, proc_params params){ //Fetch function 
if (fscanf(FP, "%lx %ld %ld %ld %ld", &read_instruction[instruction_tracker].pc, &read_instruction[instruction_tracker].INSTR, &read_instruction[instruction_tracker].dest, &read_instruction[instruction_tracker].src1, &read_instruction[instruction_tracker].src2) != EOF){
    read_instruction[instruction_tracker].FETCH = PL_cycle;
    instruction_tracker++; //counter that is used to track the number of instructions passed
}

}


void Dispatch() { //Dispatch function
    if (instruction_tracker > 3)
    read_instruction[instruction_stop].DISPATCH = PL_cycle; //sets the instruction to the pipeline cycle 
}

//RegRead function 
void RegRead() {
    if (instruction_tracker > 2)
    read_instruction[instruction_stop].REGREAD = PL_cycle; //sets the instruction to the pipeline cycle 
    }

//Rename function 
void Rename() {
    if (instruction_tracker > 1)
    read_instruction[instruction_stop].RENAME = PL_cycle; //sets the instruction to the pipeline cycle 
     }

void Retire() { //Retire function
    if (instruction_tracker > instruction_stop) {
        read_instruction[instruction_stop].RETIRE = PL_cycle; //sets the instruction to the pipeline cycle 
        instruction_stop++; //counter is incremented here, makes note of the functions that have completed 
    }
}
 //recall pipeline stages in reverse order 
int Advance_Cycle(FILE *FP, proc_params params) {
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

int main (int argc, char* argv[]){
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
    //   printf("=== Simulator Command ======\n# ./sim %lu %lu %lu %s\n", params.rob_size, params.iq_size, params.width, argv[4]);
    //   printf("=== Processor Configuration ===\n# ROB_SIZE = %lu\n# IQ_SIZE = %lu\n# WIDTH = %lu\n", params.rob_size, params.iq_size, params.width);
  
   
    //do loop that steps through each of the pipeline functions 
   do {  //THIS WAS MY ATTEMPT AT THE DO-WHILE LOOP, INCORPORATES ALL DEFINED PIPELINE FUNCTIONS AND ADVANCE_CYCLE() FUNCTION 
    Retire();
    Writeback();
    Execute();
    Issue();
    Dispatch();
    RegRead();
    Rename();
    Decode();
    Fetch(FP, params);

   } while (Advance_Cycle(FP, params)); //this occurs throughout the Advance_Cycle funciton as well. Should be stepping through each part of the function

    //THIS WAS MY ATTEMPT AT PRODUCING OUTPUTS FOR THE PIPELINE SIMULATOR
    //prints calculated results (NOTE: I KNOW IT IS INCORRECT - COULD NOT FIGURE OUT)
     printf("=== Simulation Results =======\n"); 
     printf(" Dynamic Instruction Count = %ld\n", instruction_tracker);
     printf(" Cycles = %ld\n", PL_cycle);
     printf(" Instructions Per Cycle (IPC) = %.2lf\n", (double)instruction_tracker / PL_cycle);   
  
    for (int j = 0; j < instruction_tracker; j++) {
        printf("%d fu{%ld} src{%ld,%ld} dst{%ld} FE{%ld,1} DE{%ld,1} RN{%ld,1} RR{%ld,1} DI{%ld,1} IS{%ld,1} EX{%ld,1} WB{%ld,1} RT{%ld,1}\n", j, read_instruction[j].INSTR, read_instruction[j].src1, read_instruction[j].src2, read_instruction[j].dest, read_instruction[j].FETCH, read_instruction[j].DECODE, read_instruction[j].RENAME, read_instruction[j].REGREAD, read_instruction[j].DISPATCH, read_instruction[j].ISSUE, read_instruction[j].EXECUTE, read_instruction[j].WRITEBACK, read_instruction[j].RETIRE);
    }
    //while(fscanf(FP, "%lx %d %d %d %d", &pc, &op_type, &dest, &src1, &src2) != EOF)
        //printf("%lx %d %d %d %d\n", pc, op_type, dest, src1, src2); //Print to check if inputs have been read correctly
    fclose(FP);
    return 0;
}
 
