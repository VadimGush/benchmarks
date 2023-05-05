/**
 * This benchmarks measures memory throughput by reading different amount of data.
 * The more data it reads, the more likely for that data to not be able to fit in any CPU cache,
 * which will result in lower throughput.
 */
#include <iostream>
#include <algorithm>
#include <utils/types.h>
#include <utils/system.h>
#include <limits>

// Type of data that we will read from memory in one operation
using data_type = i64;
// How many runs we will make
constexpr static u32 number_of_runs_per_test = 10;
// How much data we will read at max in bytes
constexpr static u32 data_size = 30 * MB;
// Every measurement will be performed with data size of "last measurement data size" + step_size
constexpr static u32 step_size = 100 * KB;

f64 measure(const vec<data_type>& data, const u32 measure_data_size, const u32 repeat) {
    f64 best_time = std::numeric_limits<f64>::max();

    for (u32 r = 0; r < repeat; r++) {
        sys::clock<f64> clock{};

        data_type sum = 0;
        u32 i = 0;
        for (u32 bytes = 0; bytes < measure_data_size; bytes += sizeof(data_type)) {
            sum += data[i];
            i += 1;
        }

        clock.complete();

        std::cout << "# " << sum;
        f64 time = clock.micro();

        if (time < best_time) { best_time = time; }
    }

    return best_time;
}

struct test_result {
    vec<f64> throughput{};
};

int main() {
    // Pre-allocate data that we will read
    vec<data_type> data(data_size / sizeof(data_type));
    for (u32 i = 0; i < data.size(); i++) {
        data[i] = static_cast<data_type>(i) % 2;
    }

    // Result of our benchmarks (we will select the best at the end)
    map<u32, test_result> results{};

    for (u32 run_id = 0; run_id < number_of_runs_per_test; run_id++) {
        for (u32 test_id = 1; test_id <= (data_size / step_size); test_id++) {
            const u32 repeat = 40;
            const u32 size = test_id * step_size; // bytes
            const f64 time = measure(data, size, repeat); // microseconds

            const f64 throughput = ((static_cast<f64>(size) / time) * 1'000'000.) / static_cast<f64>(GB); // GB/sec
            results[size].throughput.emplace_back(throughput);
        }
    }


    std::cout << "\nsize,throughput" << std::endl;

    for (const auto& result : results) {
        const u32 size = result.first;
        const auto& throughput = result.second.throughput;
        const auto max_throughput = *std::max_element(throughput.begin(), throughput.end());

        std::cout << (size / KB) << "," << max_throughput << std::endl;
    }

    return 0;
}
