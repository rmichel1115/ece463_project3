#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "sim_proc.h"

#define MAXIMUM 1000000

typedef struct {
    long int INSTR;
    long int src1;
    long int src2;
    long int dest;
    long int FETCH, DECODE, REGREAD, RENAME;
    long int WRITEBACK, RETIRE, EXECUTE, ISSUE, DISPATCH;
    uint64_t pc;
} ProcessorInformation;

long int instruction_tracker = 0;
long int instruction_stop = 0;
long int PL_cycle = 0;
ProcessorInformation read_instruction[MAXIMUM];

// Stages
void Fetch(FILE *FP, proc_params params) {
    if (fscanf(FP, "%lx %ld %ld %ld %ld", 
                &read_instruction[instruction_tracker].pc, 
                &read_instruction[instruction_tracker].INSTR, 
                &read_instruction[instruction_tracker].dest, 
                &read_instruction[instruction_tracker].src1, 
                &read_instruction[instruction_tracker].src2) != EOF) {
        read_instruction[instruction_tracker].FETCH = PL_cycle;
        instruction_tracker++;
    }
}
void Decode() { if (instruction_tracker > 0) read_instruction[instruction_stop].DECODE = PL_cycle; }
void RegRead() { if (instruction_tracker > 2) read_instruction[instruction_stop].REGREAD = PL_cycle; }
void Rename() { if (instruction_tracker > 1) read_instruction[instruction_stop].RENAME = PL_cycle; }
void Dispatch() { if (instruction_tracker > 3) read_instruction[instruction_stop].DISPATCH = PL_cycle; }
void Issue() { if (instruction_tracker > 4) read_instruction[instruction_stop].ISSUE = PL_cycle; }
void Execute() { if (instruction_tracker > 5) read_instruction[instruction_stop].EXECUTE = PL_cycle; }
void Writeback() { if (instruction_tracker > 6) read_instruction[instruction_stop].WRITEBACK = PL_cycle; }
void Retire() {
    if (instruction_tracker > instruction_stop) {
        read_instruction[instruction_stop].RETIRE = PL_cycle;
        instruction_stop++;
    }
}
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
    Fetch(FP, params);
    return instruction_stop < instruction_tracker;
}

int main(int argc, char* argv[]) {
    FILE *FP;
    proc_params params;
    char *trace_file;

    if (argc != 5) {
        printf("Error: Wrong number of inputs: %d\n", argc - 1);
        exit(EXIT_FAILURE);
    }

    params.rob_size = strtoul(argv[1], NULL, 10);
    params.iq_size = strtoul(argv[2], NULL, 10);
    params.width = strtoul(argv[3], NULL, 10);
    trace_file = argv[4];
    
    printf("rob_size:%lu iq_size:%lu width:%lu tracefile:%s\n",
           params.rob_size, params.iq_size, params.width, trace_file);

    FP = fopen(trace_file, "r");
    if (FP == NULL) {
        printf("Error: Unable to open file %s\n", trace_file);
        exit(EXIT_FAILURE);
    }

    while (Advance_Cycle(FP, params)) {
        printf("Cycle %ld, Fetched: %ld, Retired: %ld\n", PL_cycle, instruction_tracker, instruction_stop);
    }

    printf("=== Simulation Results ===\n");
    printf("Dynamic Instruction Count = %ld\n", instruction_tracker);
    printf("Cycles = %ld\n", PL_cycle);
    printf("Instructions Per Cycle (IPC) = %.2lf\n", (double)instruction_tracker / PL_cycle);

    for (int j = 0; j < instruction_tracker; j++) {
        printf("%d fu{%ld} src{%ld,%ld} dst{%ld} FE{%ld,1} DE{%ld,1} RN{%ld,1} RR{%ld,1} DI{%ld,1} IS{%ld,1} EX{%ld,1} WB{%ld,1} RT{%ld,1}\n",
               j, read_instruction[j].INSTR, read_instruction[j].src1, read_instruction[j].src2,
               read_instruction[j].dest, read_instruction[j].FETCH, read_instruction[j].DECODE,
               read_instruction[j].RENAME, read_instruction[j].REGREAD, read_instruction[j].DISPATCH,
               read_instruction[j].ISSUE, read_instruction[j].EXECUTE, read_instruction[j].WRITEBACK,
               read_instruction[j].RETIRE);
    }

    fclose(FP);
    return 0;
}
