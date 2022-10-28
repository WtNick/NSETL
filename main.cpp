
#include <future>
#include <iostream>
#include <thread>
#include "tasklet.h"
extern "C" {
int tasklet_main(void);
}

int main(int argc, const char* argv[])
{
    std::cout << "startin"<< std::endl;
    auto f= std::async(std::launch::async,[](){
        while(true) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            std::cout << ".";
            isr_tasklet_tick();

        }
    });
    std::cout << "tasklet main"<< std::endl;
    return tasklet_main();
}