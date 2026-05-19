#include <iostream>
#include <vector>
#include <cstring>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>

const char* SHARED_MEM_NAME = "/my_shm_barrier";
const char* SEM_BARRIER_NAME = "/sem_barrier";
const size_t SHARED_MEM_SIZE = 4096;

struct SharedData {
    int results[100];
    int num_processes;
    int ready_count;
    bool all_done;
};

class ProcessBarrier {
private:
    sem_t* sem;
    int threshold;
    
public:
    ProcessBarrier(const char* name, int num_processes) : threshold(num_processes) {
        sem = sem_open(name, O_CREAT, 0644, 0);
        if (sem == SEM_FAILED) { perror("sem_open failed"); exit(1); }
    }
    
    ~ProcessBarrier() { sem_close(sem); }
    
    void wait_for_all() {
        for (int i = 0; i < threshold; ++i) {
            sem_wait(sem);
        }
    }
    
    void signal_completion() { sem_post(sem); }
    
    static void cleanup(const char* name) { sem_unlink(name); }
};

void child_process(int id, SharedData* shared_data, ProcessBarrier& barrier) {
    int work_time = 100 + (id * 50);
    std::cout << "[Дочерний " << id << "] Начал работу..." << std::endl;
    usleep(work_time * 1000);
    
    int result = id * 100 + 13;
    shared_data->results[id] = result;
    
    std::cout << "[Дочерний " << id << "] Завершил. Результат: " << result << std::endl;
    
    barrier.signal_completion();
    exit(0);
}

int main() {
    const int NUM_PROCESSES = 5;
    
    std::cout << "Создание " << NUM_PROCESSES << " дочерних процессов..." << std::endl;
    
    int shm_fd = shm_open(SHARED_MEM_NAME, O_CREAT | O_RDWR, 0644);
    ftruncate(shm_fd, SHARED_MEM_SIZE);
    void* shm_ptr = mmap(nullptr, SHARED_MEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    SharedData* shared_data = static_cast<SharedData*>(shm_ptr);
    shared_data->num_processes = NUM_PROCESSES;
    shared_data->ready_count = 0;
    shared_data->all_done = false;
    memset(shared_data->results, 0, sizeof(shared_data->results));
    
    ProcessBarrier barrier(SEM_BARRIER_NAME, NUM_PROCESSES);
    
    std::vector<pid_t> child_pids;
    for (int i = 0; i < NUM_PROCESSES; ++i) {
        pid_t pid = fork();
        if (pid == 0) {
            child_process(i, shared_data, barrier);
        } else {
            child_pids.push_back(pid);
            std::cout << "Создан процесс PID: " << pid << std::endl;
        }
    }

    std::cout << "Ожидание завершения всех процессов..." << std::endl;
    barrier.wait_for_all();
    
    // 5. Вывод результатов
    std::cout << "\nРЕЗУЛЬТАТЫ:" << std::endl;
    int total = 0;
    for (int i = 0; i < NUM_PROCESSES; ++i) {
        std::cout << "Процесс " << i << ": " << shared_data->results[i] << std::endl;
        total += shared_data->results[i];
    }
    std::cout << "Сумма: " << total << std::endl;
    
    for (pid_t pid : child_pids) {
        waitpid(pid, nullptr, 0);
    }
    
    munmap(shm_ptr, SHARED_MEM_SIZE);
    close(shm_fd);
    shm_unlink(SHARED_MEM_NAME);
    ProcessBarrier::cleanup(SEM_BARRIER_NAME);
    
    std::cout << "Программа завершена." << std::endl;
    return 0;
}