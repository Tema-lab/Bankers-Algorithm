#include <iostream>
#include <utility>
#include <vector>
#include <algorithm>


void fill_matrix(const std::string& matrix_name, int res_types, int processes, std::vector<std::vector<int>> &matrix) {
    for(int i = 0; i < processes; ++i) {
        for(int j = 0; j < res_types; ++j) {
            std::cout << matrix_name << "[" << i << "][" << j << "]: ";
            std::cin >> matrix[i][j];
        }
    }
}

void print_matrix(int res_types, int processes, std::vector<std::vector<int>> &matrix) {
    for(int i = 0; i < processes; i++) {
        std::cout << "P" << i << "   ";
        for(int j = 0; j < res_types; j++) {
            std::cout << matrix[i][j] << "   ";
        }
        std::cout << std::endl;
    }
}

void calc_need_matrix(const std::string& matrix_name,int num_of_processes, int num_of_res_type, std::vector<std::vector<int>> &max, std::vector<std::vector<int>> &alloc, std::vector<std::vector<int>> &need) {
    std::cout << matrix_name << std::endl;
    for(int i = 0; i < num_of_processes; i++) {
        for(int k = 0; k < num_of_res_type; k++) {
            need[i][k] = max[i][k] - alloc[i][k];
        }
    }
}

bool is_in_safe_state(int num_of_processes,
                      int num_of_resources,
                      std::vector<std::vector<int>> &need_matrix,
                      std::vector<int> &avail_res,
                      std::vector<std::vector<int>> &alloc,
                      std::vector<int> &safe_sequence)
{
    std::vector<int> work = avail_res;
    std::vector<bool> finish(num_of_processes, false);
    int n = 0;
    while (true) {
        bool found = false; // Flag to indicate if a process was found

        for (int i = 0; i < num_of_processes; i++) {
            if (!finish[i]) {
                bool canAllocateProcess = true;

                for (int j = 0; j < num_of_resources; j++) {
                    if (need_matrix[i][j] > work[j]) {
                        canAllocateProcess = false;
                        break;
                    }
                }

                if (canAllocateProcess) {
                    safe_sequence[n] = i;
                    n++;
                    finish[i] = true;
                    found = true; // Set the found to true
                    for (int p = 0; p < num_of_resources; p++) {
                        work[p] += alloc[i][p];
                    }
                    break;
                }
            }
        }

        if (!found) {
            break; // If no process is found, exit the loop
        }

        bool allFinished = true;
        for (bool val : finish) {
            if (!val) {
                allFinished = false;
                break;
            }
        }

        if (allFinished) {
            return true; // All processes are finished, the system is in a safe state
        }
    }

    return false; // If the loop exits and all processes are not finished, the system is not in a safe state
}

void print_safe_sequence(int processes,std::vector<int> &safe_sequence){
    for(int i = 0; i < processes;i++){
        std::cout << "P" << safe_sequence[i];
        if(i < processes - 1){
            std::cout << " --> ";
        }else{
            std::cout << ".";
        }
    }
}

void request_of_resource(int processes, int resources, std::vector<std::vector<int>> &alloc, std::vector<std::vector<int>> &need_matrix, std::vector<int> &available,std::vector<int> &safe_sequence){
    int process_index;
    std::cout << "\nPlease enter the process index(id) for making the request >  ";
    std::cin >> process_index;

    std::vector<int> request(resources,0);

    std::cout << "Please enter request vector for process " << process_index << std::endl;

    for(int i = 0; i < resources; i++){
        std::cin >> request[i];
    }

    //to decide whether the request is granted
    for(int i = 0; i < resources; i++){
        if(request[i] > need_matrix[process_index][i]){
            std::cout << "Error! The request exceeds the maximum need of process P" << process_index << " for resource " << i << std::endl;
            return;
        }
    }

    //check if request exceeds the available resources for resource
    for(int i = 0; i < resources; i++){
        if(request[i] > available[i]){
            std::cout << "Error! The request exceeds the available resources for resource" << i << std::endl;
            return;
        }
    }

    //simulate granting the resources to the process
    for (int i = 0; i < resources; ++i) {
        available[i] -= request[i];
        alloc[process_index][i] += request[i];
        need_matrix[process_index][i] -= request[i];
    }

    if(is_in_safe_state(processes,resources, need_matrix,available,alloc,safe_sequence)){
        std::cout << "Request granted. System is in a safe state." << std::endl;
        print_safe_sequence(processes,safe_sequence);
        std::cout << "\nUpdated Allocation Matrix:" << std::endl;
        print_matrix(resources, processes, alloc);
        std::cout << "Updated Need Matrix:" << std::endl;
        print_matrix(resources, processes, need_matrix);
    }else{
        std::cout << "Request denied. System is not in safe state. " << std::endl;
        for (int i = 0; i < resources; ++i) {
            available[i] += request[i];
            alloc[process_index][i] -= request[i];
            need_matrix[process_index][i] += request[i];
        }
    }
}

int main() {
    // number of resources
    int num_of_res{0};
    // number of processes
    int num_of_processes{0};

    // matrix that stores maximum demand of each process and its type
    std::vector<std::vector<int>> max;

    // matrix that stores the number of resources of each type currently allocated to each process
    std::vector<std::vector<int>> alloc;

    // matrix that stores the remaining resource need for each process
    std::vector<std::vector<int>> need;

    // vector that stores available resources of each type
    std::vector<int> available_resources;

    std::cout << "Enter a number of processes: " << std::endl;
    std::cin >> num_of_processes;

    std::vector<int> safe_sequence;
    safe_sequence.resize(num_of_processes,0);

    std::cout << "Enter a number resources " << std::endl;
    std::cin >> num_of_res;

    max.resize(num_of_processes, std::vector<int>(num_of_res, 0));
    alloc.resize(num_of_processes, std::vector<int>(num_of_res, 0));
    need.resize(num_of_processes, std::vector<int>(num_of_res, 0));
    available_resources.resize(num_of_res);

    std::cout << "Filling allocation matrix:" << std::endl;
    fill_matrix("allocation", num_of_res, num_of_processes, alloc);

    std::cout << "Allocation matrix:" << std::endl;
    print_matrix(num_of_res, num_of_processes, alloc);

    std::cout << "Filling max matrix:" << std::endl;
    fill_matrix("max", num_of_res, num_of_processes, max);

    std::cout << "Max matrix:" << std::endl;
    print_matrix(num_of_res, num_of_processes, max);

    std::cout << "Fill vector of available resources" << std::endl;
    for (int i = 0; i < num_of_res; i++) {
        std::cout << "Available resources for resource " << i << ": ";
        std::cin >> available_resources[i];
    }
    std::cout << "" << std::endl;

    std::cout << "Available resources: ";
    for (int i = 0; i < num_of_res; i++) {
        std::cout <<  available_resources[i] << " ";
    }

    // Check whether it’s possible to allocate.
    calc_need_matrix("\nNeed Matrix",num_of_processes, num_of_res, max, alloc, need);
    print_matrix(num_of_res,num_of_processes,need);

    if (is_in_safe_state(num_of_processes,num_of_res,need,available_resources,alloc,safe_sequence)) {
        std::cout << "System is in a safe state." << std::endl;
        print_safe_sequence(num_of_processes,safe_sequence);
    } else {
        std::cout << "System is not in a safe state." << std::endl;
        return 0;
    }

    request_of_resource(num_of_processes,num_of_res,alloc,need,available_resources,safe_sequence);

    return 0;
}
