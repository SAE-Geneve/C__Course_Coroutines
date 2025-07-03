#include <algorithm>
#include <array>
#include <coroutine>
#include <iostream>
#include <random>
#include <vector>

constexpr int NUM_WORKERS = 3;
std::array<int, NUM_WORKERS> results;

// Simple task for coroutines
struct Task {
    struct promise_type {
        Task get_return_object(){
            return Task{std::coroutine_handle<promise_type>::from_promise(*this)};
        };
        std::suspend_always initial_suspend(){ return {}; }
        std::suspend_always final_suspend() noexcept{ return {}; }

        void return_void(){
        }

        void unhandled_exception(){
        }
    };

    explicit Task(std::coroutine_handle<promise_type> h) : handle(h){
    }

    // Destrutor
    ~Task(){
        handle.destroy();
    }

    std::coroutine_handle<promise_type> handle;
};


// Coroutine that increments counter
Task increment_counter(int times, int idx, int &counter){
    for (int counter_loop = 0; counter_loop < times; ++counter_loop) {
        counter++;
        std::cout << "Worker " << idx << " : " << counter << "/" << times << std::endl;
        co_await std::suspend_always{};
    }
    std::cout << "Worker " << idx << " done" << std::endl;
    co_return;
}

int main(){
    std::random_device rd;  // Obtient une source d'entropie matérielle
    std::mt19937 gen(rd()); // Initialise le générateur de nombres aléatoires
    std::uniform_int_distribution<> dis(1, 10); // Distribution uniforme entre 1 et 10
    
    std::cout << "Starting parallel counter...\n";

    std::array tasks_ = {
        increment_counter(dis(gen), 0, results[0]),
        increment_counter(dis(gen), 1, results[1]),
        increment_counter(dis(gen), 2, results[2]),
    };

    bool all_is_done;
    do {
        all_is_done = true;

        for (int i = 0; i < NUM_WORKERS; ++i) {

            if (!tasks_[i].handle.done()) {
                tasks_[i].handle.resume();
            }else {
                std::cout << "Task [" << i << "] done " << std::endl;
            }

            all_is_done &= tasks_[i].handle.done();

        }

        std::cout << "Doing something else (All is done ?) : " << (all_is_done ? "True" : "False") << std::endl;
    } while (!all_is_done);

    std::cout << "All task done !" << std::endl;

    return 0;

}