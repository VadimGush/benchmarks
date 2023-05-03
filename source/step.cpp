/**
 * This benchmark measures time to access (update) every Nth byte in an array.
 *
 * There are 2 build parameters to this benchmark:
 *  * POLLUTE - define if you want to pollute cache before every measure
 *  * SUM - perform a sum of bytes from the array
 *  * UPDATE - update every byte in the array
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

#ifdef UPDATE
    u32 i = 0;
    for (u32 bytes = 0; bytes < total; bytes += 1) {
        data[i] += 1;
        i += step;
    }
#endif

#ifdef SUM
    u32 sum = 0;
    u32 i = 0;
    for (u32 bytes = 0; bytes < total; bytes += 1) {
        sum += data[i];
        i += step;
    }
#endif

    // Measure how much time it took us to execute
    const auto end = std::chrono::high_resolution_clock::now();
    const std::chrono::duration<f64, std::micro> duration = end - start;

#ifdef SUM
    std::cout << '#' << sum << '\n';
#endif

    return duration.count();
}

constexpr u32 total_bytes_read = 1 * MB;

struct test_result {
    std::vector<f64> time{};
    u32 step = 0;
};

static std::vector<u8> garbage(50 * MB);
void pollute_cache() {
    for (u32 i = 0; i < garbage.size(); i++) { garbage[i] += i; }
    u32 sum = 0;
    for (const auto& element : garbage) { sum += element; }
    std::cout << "#" << sum << std::endl;
}

int main() {
    // Every test we will run 5 times to avoid noise
    constexpr u32 number_of_runs_per_test = 5;
    std::vector<test_result> test_results(256);

    // Pre-allocate data
    std::vector<u8> data(test_results.size() * total_bytes_read);
    for (u32 i = 0; i < data.size(); i++) { data[i] += i; }

    // Execute benchmarks (tests)
    for (u32 run_id = 0; run_id < number_of_runs_per_test; run_id++) {
        for (u32 test_id = 0; test_id < test_results.size(); test_id++) {
            const u32 step = test_id + 1;

#ifdef POLLUTE
            pollute_cache();
#endif

            const f64 execution_time = test(data, step, total_bytes_read);

            test_results[test_id].time.emplace_back(execution_time);
            test_results[test_id].step = step;
        }
    }

    // Print results in CSV format
    // names of the columns first
    std::cout << "step,time" << std::endl;
    for (const auto& test_result : test_results) {
        // we will select the fastest execution time
        const f64 execution_time = *std::min_element(test_result.time.begin(), test_result.time.end());
        std::cout << test_result.step << "," << execution_time << '\n';
    }

    return 0;
}