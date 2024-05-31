struct Runtime_result;

void printResult(Runtime_result result, int pid);

struct Runtime_result{
    int start_time = -1;
    int end_time;
    int waiting_time;
    int turnaround_time; //end time - arrival time
    int execution_time = 0; // used for RR
    void (*printStruct)(Runtime_result, int) = printResult; //function pointer just for fun
};
