#include <iostream>
#include <list>
#include "process.h"

#define SHOW_EXECUTION false
#define SHOW_FINISHED true
#define SHOW_RESULT true

void printResult(Runtime_result result, int pid){
    std::cout << "PID: " << pid << " | Start Time: " << result.start_time << " | End Time: " << result.end_time << " | Waiting Time: " << result.waiting_time << " | Turnaround Time: " << result.turnaround_time << std::endl;
}

void showFinished(int pid){
    std::cout << "PID: " << pid << " | Finished" << std::endl;
}

class Process{
    private:
        int pid;
        int arrival_time;
        int burst_time;
        Runtime_result runtime_result;

    public:
        Process(int pid, int arrival_time, int burst_time){
            this->pid = pid;
            this->arrival_time = arrival_time;
            this->burst_time = burst_time;
        }

        [[nodiscard]] int get_pid() const {
            return pid;
        }

        [[nodiscard]] int get_arrival_time() const{
            return arrival_time;
        }

        [[nodiscard]] int get_burst_time() const{
            return burst_time;
        }

        [[maybe_unused]] void set_burst_time(int burst_time){
            this->burst_time = burst_time;
        }

        [[maybe_unused]] void set_arrival_time(int arrival_time){
            this->arrival_time = arrival_time;
        }

        [[maybe_unused]] void set_pid(int pid){
            this->pid = pid;
        }

        void set_runtime_result(Runtime_result runtime_result){
            this->runtime_result = runtime_result;
        }

        Runtime_result& get_runtime_result(){
            return runtime_result;
        }
};

void FCFS(std::list<Process> processList){
    auto iterator = processList.begin(); //std::list<Process>::iterator
    auto copy = iterator; //keep a copy of previous iterator in case the original iterator changes
    while(iterator != processList.end()){
        if(iterator->get_arrival_time() == 0){
            #if SHOW_EXECUTION
                std::cout << "Process " << iterator->get_pid() << " is executed" << std::endl;
            #endif
            iterator->set_runtime_result({0, iterator->get_burst_time(), 0, iterator->get_burst_time()});

            #if SHOW_FINISHED
                showFinished(iterator->get_pid());
            #endif
        }else{
            #if SHOW_EXECUTION
                std::cout << "Process " << iterator->get_pid() << " is executed" << std::endl;
            #endif

            //in case the process is executed before the arrival time due to the previous process has already been finished
            int start_time;
            if(copy->get_runtime_result().end_time >= iterator->get_arrival_time()){
                start_time = copy->get_runtime_result().end_time;
            }else{
                start_time = iterator->get_arrival_time();
            }

            iterator->set_runtime_result({start_time, start_time + iterator->get_burst_time(), start_time - iterator->get_arrival_time(), start_time + iterator->get_burst_time() - iterator->get_arrival_time()});
            #if SHOW_FINISHED
                showFinished(iterator->get_pid());
            #endif
            copy ++;
            
        }
        iterator ++;
    }
    #if SHOW_RESULT
    std::cout << "--------------------------------------------------------------------------------------" << std::endl;
        for(auto& process : processList){
            printResult(process.get_runtime_result(), process.get_pid());
        }
    #endif
}

void RR(std::list<Process> processList, int quantum){
    std::list<Process> finished_processes;
    auto iterator = processList.begin();
    int current_time = 0;
    while(!processList.empty()){
        Runtime_result& result_reference = iterator->get_runtime_result();
        if(result_reference.start_time == -1 && iterator->get_arrival_time() >= current_time){
            result_reference.start_time = iterator->get_arrival_time();
            current_time = iterator->get_arrival_time();
            //this is able to deal the issue when first arrival time is set to 0
        }else if(result_reference.start_time == -1){
            result_reference.start_time = current_time;
        }
        #if SHOW_EXECUTION
            std::cout << "Process " << iterator->get_pid() << " is executed" << std::endl;
        #endif
        if(iterator->get_burst_time() <= quantum){
            result_reference.end_time = current_time + iterator->get_burst_time();
            result_reference.execution_time = iterator->get_burst_time();
            result_reference.turnaround_time = result_reference.end_time - iterator->get_arrival_time();
            result_reference.waiting_time = result_reference.turnaround_time - result_reference.execution_time;
            finished_processes.push_back(*iterator);
            #if SHOW_FINISHED
                showFinished(iterator->get_pid());
            #endif
            iterator = processList.erase(iterator);
            current_time += iterator->get_burst_time();
        }else if(iterator->get_burst_time() > quantum){
            if(result_reference.execution_time + quantum < iterator->get_burst_time()){
                result_reference.execution_time += quantum;
                current_time += quantum;
                iterator ++;
            }else{
                result_reference.end_time = current_time + (iterator->get_burst_time() - result_reference.execution_time);
                current_time += iterator->get_burst_time() - result_reference.execution_time;
                result_reference.execution_time = iterator->get_burst_time();
                result_reference.turnaround_time = result_reference.end_time - iterator->get_arrival_time();
                result_reference.waiting_time = result_reference.turnaround_time - result_reference.execution_time;
                finished_processes.push_back(*iterator);
                #if SHOW_FINISHED
                    showFinished(iterator->get_pid());
                #endif
                iterator = processList.erase(iterator);
            }
        }
        if(iterator == processList.end()){
            //iterator won't return to the beginning of the list, it will become a past-the-end iterator, so we need to reset it
            iterator = processList.begin();
        }
    }
    #if SHOW_RESULT
    std::cout << "--------------------------------------------------------------------------------------" << std::endl;
        for(auto process : finished_processes){
            printResult(process.get_runtime_result(), process.get_pid());
        }
    #endif
}

int main(){
    std::list<Process> processList;
    //arrival time of the first process must be 0 in FCFS
    while(true){
        std::cout << "Enter the process details (pid, arrival_time, burst_time) or use default datasets(press d): ";
        if(std::cin.peek() == 'd'){
            processList.emplace_back(1, 0, 5);
            processList.emplace_back(2, 1, 5);
            processList.emplace_back(3, 2, 3);
            processList.emplace_back(4, 3, 1);
            processList.emplace_back(5, 100, 200);
            processList.emplace_back(6, 101, 3);
            break;
        }else{
            int pid, arrival_time, burst_time;
            std::cin >> pid >> arrival_time >> burst_time;
            processList.emplace_back(pid, arrival_time, burst_time);
            std::cout << "Do you want to enter another process? (y/n): ";
            char choice;
            std::cin >> choice;
            if(choice == 'n'){
                break;
            }
        }
    }

    std::cout << "-----------------------------------------FCFS-----------------------------------------" << std::endl;
    FCFS(processList);
    std::cout << "--------------------------------------------------------------------------------------" << std::endl;
    std::cout << "-----------------------------------------RR--------------------------------------------" << std::endl;
    RR(processList, 2);
    std::cout << "--------------------------------------------------------------------------------------" << std::endl;
}

