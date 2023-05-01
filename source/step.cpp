/**
 * This benchmark measures time to access (update) every Nth byte in an array.
 * It performs multiple benchmarks with
 */
#include <iostream>
#include <vector>
#include <chrono>

using u32 = uint32_t;
using u8 = uint8_t;
using f32 = float;
using f64 = double;

constexpr u32 KB = 1024;
constexpr u32 MB = 1024 * 1024;
constexpr u32 GB = 1024 * 1024 * 1024;

/**
 * This code updates every Nth byte (where N - defined by the provided "step" value)
 * in a given array until it will read enough bytes specified by "total" value.
 */
f64 test(std::vector<u8>& data, const u32 step, const u32 total) {
    const auto start = std::chrono::high_resolution_clock::now();

    u32 i = 0;
    for (u32 bytes = 0; bytes < total; bytes += 1) {
        data[i] += 1;
        i += step;
    }

    // Measure how much time it took us to execute
    const auto end = std::chrono::high_resolution_clock::now();
    const std::chrono::duration<f64, std::micro> duration = end - start;
    return duration.count();
}

constexpr u32 total_bytes_read = 1 * MB;
std::vector<u8> garbage(50 * MB);

void pollute_cache() {
    for (u32 i = 0; i < garbage.size(); i++) { garbage[i] += i; }
    u32 sum = 0;
    for (unsigned char i : garbage) { sum += i; }
    std::cout << "# " << sum << std::endl;
}

f64 benchmark(const u32 step) {
    /*
     * We will allocate some data in memory and try to read it. Because
     * we want to measure how much time it takes to access elements of the array
     * which is not in CPU cache currently, we will try to pollute cache after array
     * of data will be created.
     */

    // Allocate data that we will read
    std::vector<u8> data(total_bytes_read * step);
    for (u32 i = 0; i < data.size(); i++) { data[i] += i; }

    // Pollute cache by process some another array which size
    // is larger than size of CPU cache.
    pollute_cache();

    // Perform reads with a given step
    const f64 test_result = test(data, step, total_bytes_read);

    // This is probably not needed. I've just added it here for the compiler
    // to not remove everything related to the vector<u8> data, because otherwise
    // it will not be used anywhere and can be eliminated from the binary completely.
    std::cout << "# " << *std::min_element(data.begin(), data.end()) << std::endl;

    return test_result;
}

struct test_result {
    std::vector<f64> time{};
    u32 step = 0;
};

int main() {

    // Every test we will run 5 times to avoid noise
    constexpr u32 number_of_runs_per_test = 5;
    std::vector<test_result> test_results(256);

    // Execute benchmarks (tests)
    for (u32 run = 0; run < number_of_runs_per_test; run++) {
        for (u32 test = 0; test < test_results.size(); test++) {
            const u32 step = test + 1;
            const f64 execution_time = benchmark(step);
            test_results[test].time.emplace_back(execution_time);
            test_results[test].step = step;
        }
    }

    // Print results in CSV format
    // names of the columns first
    std::cout << "step,time" << std::endl;
    for (const auto& test_result : test_results) {
        // we will select the fastest execution time
        const f64 execution_time = *std::min_element(test_result.time.begin(), test_result.time.end());
        std::cout << test_result.step << "," << execution_time << std::endl;
    }

    return 0;
}