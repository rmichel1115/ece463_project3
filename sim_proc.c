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

//intialize variables 
double instructions_per_cycle = 0;
uint64_t track_instructions = 0;
uint64_t track_cycle = 0;
long int finish_file = 0;
PL processor_PL;

//function below begins the pipeline 

void PL_Start(proc_params params){
    processor_PL.width = params.width;
    processor_PL.iq_size = params.iq_size;
    processor_PL.rob_size = params.rob_size; 
    processor_PL.PL_cycle = 0;
    processor_PL.CO_read = malloc(params.rob_size * sizeof(PL_instructions));
    processor_PL.EX_read = malloc(params.width * sizeof(PL_instructions));
    processor_PL.IS_read = malloc(params.rob_size * sizeof(PL_instructions));
    processor_PL.FE_read = malloc(params.width * sizeof(PL_instructions));
}

//defining functions for pipeline simulation (reverse order)

void Commit(){
    printf("Instructions Committed\n");
}

void Execute() {
    printf("Instructions Executed\n");
}

void Issue(proc_params params){
    printf("%lu instructions\n", params.width / 2);
}

void Decode(proc_params params){
    printf("%lu instructions\n", params.width);

}

void Fetch(FILE *FP, proc_params params){
    for (int j = 0; j < !finish_file && params.width; j++) {
        if (scanf(FP, "%lx %d %d %d %d", &processor_PL.FE_read[j].pc, &processor_PL.FE_read[j].op_type, &processor_PL.FE_read[j].dest, &processor_PL.FE_read[j].src1, &processor_PL.FE_read[j].src2) == EOF){
            finish_file = 1;
        }
            
    }
}

int Advance_Cycle(FILE *FP, proc_params params) {

    Commit();
    Execute();
    Issue(params);
    Decode(params);
    Fetch(FP, params);
    processor_PL.PL_cycle++; //counter for pipeline
    return !finish_file;

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
   
   PL_Start(params);
   do {
    Advance_Cycle(FP, params);
    track_instructions += params.width;
   
  } while (!finish_file);

    instructions_per_cycle = track_instructions / processor_PL.PL_cycle;

    printf("Cycles: %lu Instructions: %.u Instructions Per Cycle: %.2f\n", processor_PL.PL_cycle, track_instructions, instructions_per_cycle);
    fclose(FP); 
    //while(fscanf(FP, "%lx %d %d %d %d", &pc, &op_type, &dest, &src1, &src2) != EOF)
        //printf("%lx %d %d %d %d\n", pc, op_type, dest, src1, src2); //Print to check if inputs have been read correctly

    return 0;
}

