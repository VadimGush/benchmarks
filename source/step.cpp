/**
 * This benchmark reads fixed amount of bytes from an array with different step ("skip" value).
 * Through measures, the step will change from 1 (skipping 0 bytes before reading the next one)
 * to 256 (skipping 255 bytes before reading the next one).
 *
 * There are 3 compile definitions to this benchmark that you can specify:
 *   POLLUTE    - define if you want to pollute cache before every measure (enabled by default)
 *   SUM        - perform a sum of bytes from the array (doesn't involve writes to the data)
 *   UPDATE     - update every byte in the array (might be slower because of writes)
 */
#include <iostream>
#include <vector>
#include <utils/types.h>
#include <utils/system.h>

// Type of data that we will read from array (byte - by default)
using data_type = u8;
// How much times we will run every measurement
constexpr u32 number_of_runs_per_test = 5;
// How much actual data we will read in total (skipped data not included)
constexpr u32 total_bytes_read = 1 * MB;

/**
 * Measures time to perform some operation on every Nth byte in the provided array.
 * Returns total execution time.
 */
f64 measure(std::vector<data_type>& data, const u32 step, const u32 total) {
    sys::clock<f64> clock{};

#ifdef UPDATE
    u32 i = 0;
    for (u32 bytes = 0; bytes < total; bytes += sizeof(data_type)) {
        data[i] += 1;
        i += step;
    }
#endif

#ifdef SUM
    data_type sum = 0;
    u32 i = 0;
    for (u32 bytes = 0; bytes < total; bytes += sizeof(data_type)) {
        sum += data[i];
        i += step;
    }
#endif

    clock.complete();

#ifdef SUM
    std::cout << '#' << sum << '\n';
#endif

    return clock.micro();
}


struct test_result {
    std::vector<f64> time{};
    u32 step = 0;
};

/**
 * This code tries to pollute the cache by processing some unrelated data.
 * In this case simply an array of 100 MB of data.
 */
static std::vector<i64> garbage(100 * MB / sizeof(i64));
void pollute_cache() {
    for (u32 i = 0; i < garbage.size(); i++) { garbage[i] += static_cast<i64>(i); }
    i64 sum = 0;
    for (const auto& element : garbage) { sum += element; }
    std::cout << "#" << sum << std::endl;
}

int main() {
    std::vector<test_result> test_results(256 / sizeof(data_type));

    // Pre-allocate array that we will read (process)
    std::vector<data_type> data((test_results.size() * total_bytes_read) / sizeof(data_type));
    for (u32 i = 0; i < data.size(); i++) { data[i] += static_cast<data_type>(i); }

    // Execute benchmarks (tests)
    for (u32 run_id = 0; run_id < number_of_runs_per_test; run_id++) {
        for (u32 test_id = 0; test_id < test_results.size(); test_id++) {
            const u32 step = test_id + 1;

            // We need to pollute CPU cache in order to avoid previous measures
            // affecting the current one.
#ifdef POLLUTE
            pollute_cache();
#endif

            const f64 execution_time = measure(data, step, total_bytes_read);

            // Write down measurements
            test_results[test_id].time.emplace_back(execution_time);
            test_results[test_id].step = step;
        }
    }

    // Print results in CSV format
    // names of the columns first
    std::cout << "step,time" << std::endl;
    for (const auto& test_result : test_results) {
        // We will select the fastest execution time
        const f64 execution_time = *std::min_element(test_result.time.begin(), test_result.time.end());
        std::cout << test_result.step << "," << execution_time << '\n';
    }

    return 0;
}