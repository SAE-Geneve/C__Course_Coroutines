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
    SimpleCoroutine(std::coroutine_handle<promise_type> h) : handle(h) {}

    // Destructor cleans up the coroutine
    ~SimpleCoroutine() {
        if (handle) handle.destroy();
    }

    // Move constructor and assignment (coroutines can't be copied)
    SimpleCoroutine(SimpleCoroutine&& other) noexcept : handle(other.handle) {
        other.handle = {};
    }
    SimpleCoroutine& operator=(SimpleCoroutine&& other) noexcept {
        if (this != &other) {
            if (handle) handle.destroy();
            handle = other.handle;
            other.handle = {};
        }
        return *this;
    }

    // Delete copy constructor and assignment
    SimpleCoroutine(const SimpleCoroutine&) = delete;
    SimpleCoroutine& operator=(const SimpleCoroutine&) = delete;

    std::coroutine_handle<promise_type> handle;
};

// This is our coroutine function
SimpleCoroutine example_coroutine() {
    std::cout << "Coroutine: Starting execution\n";

    std::cout << "Coroutine: About to suspend...\n";
    co_await std::suspend_always{};  // Suspend here

    std::cout << "Coroutine: Resumed! Continuing execution...\n";

    std::cout << "Coroutine: About to suspend again...\n";
    co_await std::suspend_always{};  // Suspend again

    std::cout << "Coroutine: Resumed again! Finishing up...\n";
}

int main() {
    std::cout << "Main: Creating coroutine\n";

    // Create the coroutine (it starts executing immediately due to initial_suspend)
    auto coro = example_coroutine();

    std::cout << "Main: Coroutine created and suspended\n";

    // Resume the coroutine
    std::cout << "Main: Resuming coroutine first time\n";
    coro.handle.resume();

    std::cout << "Main: Coroutine suspended again\n";

    // Resume the coroutine again
    std::cout << "Main: Resuming coroutine second time\n";
    coro.handle.resume();

    std::cout << "Main: Coroutine finished\n";

    return 0;
}