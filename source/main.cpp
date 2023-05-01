//
// Created by Vadim Gush on 01.05.2023.
//
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
 * This function will actually test the speed of reads for a particular configuration.
 *
 * @param data data in memory that we will read
 * @param step how many bytes we will skip before reading the next byte
 * @param offset position in the data where we will start reading data
 * @param total total number of bytes to read
 * @return sum of all bytes (u8 values)
 */
std::pair<u32, f64> test(const std::vector<u8>& data, const u32 step, const u32 total) {

    // We need to actually do something with data that we read, so we will just perform a sum.
    u32 sum = 0;

    const auto start = std::chrono::high_resolution_clock::now();

    u32 i = 0;
    for (u32 bytes = 0; bytes < total; bytes += 1) {
        sum += data[i];
        i += step;
    }

    // Measure how much time it took us to execute
    const auto end = std::chrono::high_resolution_clock::now();
    const std::chrono::duration<f64, std::micro> duration = end - start;

    // Don't forget to return the sum itself. Otherwise, compiler might remove our for loop
    // completely because the result of its calculations will not be used anywhere.
    return { sum, duration.count() };
}

constexpr u32 total_read = 1 * MB;
std::vector<u8> garbage(50 * MB);

void pollute_cache() {
    for (u32 i = 0; i < garbage.size(); i++) { garbage[i] += i; }
    u32 sum = 0;
    for (unsigned char i : garbage) { sum += i; }
    std::cout << "# " << sum << std::endl;
}

f64 bench(const u32 step) {
    // Allocate data that we will read
    std::vector<u8> data(total_read * step);
    for (u32 i = 0; i < data.size(); i++) { data[i] += i; }

    pollute_cache();

    // Perform reads with a given step
    const auto test_result = test(data, step, total_read);

    // Don't forget to print the sum, otherwise for loop inside the test()
    // function might be removed by the compiler.
    std::cout << "# " << test_result.first << std::endl;

    // Return how much time we spend
    return test_result.second;
}

int main() {

    // We will output all data in CSV format to standard output.
    // So first of all we should name the columns in our data.
    std::cout << "step,time" << std::endl;

    constexpr u32 number_of_runs_per_test = 5;

    std::vector<f64> results(64);

    for (u32 t = 0; t < results.size(); t++) {
        auto& time = results[t];
        for (u32 run = 0; run < number_of_runs_per_test; run++) {
            time += bench((t + 1) * 2);
        }
        time /= number_of_runs_per_test;
    }

    for (u32 i = 0; i < results.size(); i += 1) {
        std::cout << (i + 1) * 2 << "," << results[i] << std::endl;
    }

    return 0;
}