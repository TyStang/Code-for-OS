#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <unistd.h>
#include <sys/mman.h>

const size_t PAGE_SIZE = 4096;
const size_t NUM_PAGES = 2;

std::mutex console_mutex;

void isolated_function(int id, void* memory_space) {
    std::lock_guard<std::mutex> lock(console_mutex);
    std::cout << "Thread " << id << " accessing its isolated memory space." << std::endl;

    // Access the memory space (write data to it)
    char* mem_space = static_cast<char*>(memory_space);
    for (size_t i = 0; i < PAGE_SIZE; ++i) {
        mem_space[i] = 'A' + id;
    }

    // Verify memory isolation (read and print data)
    std::cout << "Thread " << id << " has written: " << std::string(mem_space, 10) << "..." << std::endl;

    // Pause to simulate process runtime
    std::this_thread::sleep_for(std::chrono::seconds(1));
}

int main() {
    std::vector<void*> isolated_memory_spaces;

    // Allocate isolated memory spaces for threads
    for (int i = 0; i < NUM_PAGES; ++i) {
        void* memory_space = mmap(
            nullptr, 
            PAGE_SIZE, 
            PROT_READ | PROT_WRITE, 
            MAP_PRIVATE | MAP_ANONYMOUS, 
            -1, 
            0
        );

        if (memory_space == MAP_FAILED) {
            perror("mmap failed");
            return 1;
        }

        isolated_memory_spaces.push_back(memory_space);
    }

    // Spawn threads to use the isolated memory spaces
    std::vector<std::thread> threads;
    for (int i = 0; i < NUM_PAGES; ++i) {
        threads.emplace_back(isolated_function, i, isolated_memory_spaces[i]);
    }

    // Wait for all threads to complete
    for (auto& thread : threads) {
        thread.join();
    }

    // Cleanup memory
    for (void* memory_space : isolated_memory_spaces) {
        if (munmap(memory_space, PAGE_SIZE) != 0) {
            perror("munmap failed");
        }
    }

    return 0;
}
