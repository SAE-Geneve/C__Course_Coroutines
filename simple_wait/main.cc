#include <coroutine>
#include <iostream>



// A simple coroutine that can be suspended and resumed
struct SimpleCoroutine {

    // The promise_type is required by the C++ coroutine machinery
    struct promise_type {
        SimpleCoroutine get_return_object() {
            return SimpleCoroutine{std::coroutine_handle<promise_type>::from_promise(*this)};
        }
        std::suspend_never initial_suspend() { return {}; }  // Start immediately
        std::suspend_never final_suspend() noexcept { return {}; }  // Don't suspend at end
        void unhandled_exception() {}
        void return_void() {}
    };

    // Constructor takes a coroutine handle
    explicit SimpleCoroutine(std::coroutine_handle<promise_type> h) : handle(h) {}

    std::coroutine_handle<promise_type> handle;

};

// This is our coroutine function
SimpleCoroutine example_coroutine() {
    int data = 0;

    std::cout << "Coroutine: Starting\n";

    data++;
    std::cout << "Coroutine: Step 1 :" << data << "\n";
    co_await std::suspend_always{};  // Suspend here

    std::cout << "Coroutine: Step 2 :" << data << "\n";
    for (int increment_data = 0; increment_data < 5; ++increment_data) {
        data ++;
    }

    std::cout << "Coroutine: Step 3 :" << data << "\n";
    co_await std::suspend_always{};  // Suspend again

    std::cout << "Coroutine: Ended\n";

}

int main() {
    std::cout << "Main: Start\n";

    // Create the coroutine (it starts executing immediately due to initial_suspend)
    auto coro = example_coroutine();

    std::cout << "Main: Step 1\n";

    // Resume the coroutine
    std::cout << "Main: Step 2\n";
    coro.handle.resume();

    std::cout << "Main: Step 3\n";

    // Resume the coroutine again
    std::cout << "Main: Step 4\n";
    coro.handle.resume();

    std::cout << "Main: End\n";

    return 0;

}