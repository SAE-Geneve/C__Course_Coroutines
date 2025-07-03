#include <coroutine>
#include <iostream>
#include <vector>

// Simple task for coroutines
struct Task {
    struct promise_type {
        Task get_return_object(){
            return Task{std::coroutine_handle<promise_type>::from_promise(*this)};
        };
        std::suspend_never initial_suspend(){ return {}; }
        std::suspend_never final_suspend() noexcept{ return {}; }
        void return_void(){}
        void unhandled_exception(){}
    };

    std::suspend_never yield_value(){
        return std::suspend_never{};
    }

    explicit Task(std::coroutine_handle<promise_type> h) : handle(h){
    }

    std::coroutine_handle<promise_type> handle;

    bool is_done = false;

};


std::vector<int> results;

// Coroutine that increments counter
Task increment_counter(int times, int id){
    for (int i = 0; i < times; ++i) {
        results[id]++;
        std::cout << "Worker " << id << ": " << results[id] << std::endl;
        co_await std::suspend_always{};
    }
    co_return;
}

int main(){
    std::cout << "Starting parallel counter...\n";

    std::vector<Task> tasks_;

    for (int i = 0; i < 3; ++i) {
        results.emplace_back(0);
        auto co_rtine = increment_counter(10, i);
        tasks_.emplace_back(co_rtine);
    }

    bool all_is_done = true;

    do {
        for (auto &task: tasks_) {
            task.handle.resume();
            all_is_done &= task.is_done;
        }

        std::cout << "Doing something else" << std::endl;

    } while (!all_is_done);

    std::cout << "All task done !" << std::endl;

    return 0;
}
