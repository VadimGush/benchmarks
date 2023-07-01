/**
 * This benchmarks measures memory throughput by reading different amount of data linearly.
 * The more data it reads, the more likely for that data to not be able to fit in any CPU cache,
 * which will result in lower throughput.
 */
#include <iostream>
#include <algorithm>
#include <utils/types.h>
#include <utils/system.h>

// Type of data that we will read from memory in one operation
using data_type = i64;
// How many runs we will make per test
constexpr static u32 number_of_runs_per_test = 200;
// How much data we will read at max in bytes
constexpr static u32 data_size = 50 * MB;
// Every measurement will be performed with data size of "last measurement data size" + step_size
constexpr static u32 step_size = 10 * MB;

f64 measure(const vec<data_type>& data, const u32 range) {
    sys::clock<f64> clock{};

    data_type sum = 0;
    u32 id = 0;
    while (true) {
        sum += data[id];

        id += 1;
        if (id >= range) break;
    }

    clock.complete();

    // Compiler will remove computations related to our data if the result
    // of that computations is not used. For this reason we will print the sum.
    std::cout << sum % 10;

    return clock.micro();
}

struct test_result {
    u32 range{};
    f64 throughput{};
};

vec<data_type> create_data(u32 t) {
    vec<data_type> data(data_size / sizeof(data_type));
    for (u32 i = 0; i < data.size(); i++) {
        // just fill with some data
        data[i] = static_cast<data_type>(i) % (t + 10);
    }
    return data;
}

int main() {
    // Result of our benchmarks (we will select the best at the end)
    map<u32, vec<test_result>> results{};


    std::cout << "#"; // whatever we'll print in tests, should be ignored
    for (u32 test_id = 1; test_id <= (data_size / step_size); test_id++) {
        vec<data_type> data = create_data(test_id);

        for (u32 run_id = 0; run_id < number_of_runs_per_test; run_id++) {
            const u32 range = test_id * step_size; // bytes
            const f64 time_micro = measure(data, range / sizeof(data_type));

            const f64 throughput = ((static_cast<f64>(range) / time_micro) * 1'000'000.) / static_cast<f64>(GB);
            results[test_id].emplace_back(test_result{ .range = range, .throughput = throughput });
        }
    }


    std::cout << "\nsize,throughput\n";

    for (const auto& result : results) {
        const auto test_results = result.second;
        const auto it = std::max_element(test_results.begin(), test_results.end(),
          [](const test_result& a, const test_result& b) { return a.throughput < b.throughput; });

        std::cout << it->range / KB << "," << it->throughput << "\n";
    }

    return 0;
}
