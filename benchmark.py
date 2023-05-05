# This script will build and run all benchmarks, then save
# the results as PNG files to the ./results directory.
#
# CSV files generated by benchmarks you can find in ./build directory.

import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import os
import platform
import sys
from datetime import datetime

# Construct a name for directory where we will store benchmarks results
now = datetime.now()
results_directory = now.isoformat().split("T")[0] + "-"
results_directory += str(now.hour) + str(now.minute) + str(now.second) + "-" + platform.machine()
results_directory = "./results/" + results_directory


# Compile and build all benchmarks
# This function will also create directories where we will store out results
def build():
    print("-> Compiling benchmarks...\n")

    os.system("cmake -DCMAKE_BUILD_TYPE=Release -S . -B build")
    os.system("cd build; make -j8")
    os.system("mkdir results; mkdir " + results_directory)

    print("\n-> Compilation complete!")


def step(type, title):
    name = "step_" + type
    csv_filename = results_directory + "/" + name + ".csv"
    os.system("./build/" + name + " > " + csv_filename)

    df = pd.read_csv(csv_filename, comment='#')
    x = df['step']
    y = df['time']
    fig, ax = plt.subplots()
    ax.plot(np.asarray(x, float), y)
    ax.set_xlabel("step (N)")
    ax.set_ylabel("time (microseconds)")
    ax.grid(True)
    ax.set_title(title)

    plt.savefig(results_directory + "/" + name + ".png", dpi=300)


def throughput(title):
    name = "throughput"
    csv_filename = results_directory + "/" + name + ".csv"
    os.system("./build/" + name + " > " + csv_filename)

    df = pd.read_csv(csv_filename, comment='#')
    x = df['size']
    y = df['throughput']
    fig, ax = plt.subplots()
    ax.plot(np.asarray(x, float), y)
    ax.set_xlabel("size (KB)")
    ax.set_ylabel("throughput (GB/sec)")
    ax.grid(True)
    ax.set_title(title)

    plt.savefig(results_directory + "/" + name + ".png", dpi=300)


class benchmark:
    def __init__(self, name, description, call):
        self.name = name
        self.description = description
        self.call = call


benchmarks = {
    "throughput": benchmark(
        "Memory Throughput",
        "Measures memory throughput by reading data of different sizes",
        lambda: throughput("Memory Throughput")
    ),
    "step-sum": benchmark(
        "Sum every Nth byte of an array",
        "Performs a sum of every Nth byte of an array with some defined step",
        lambda: step("sum", "Sum every Nth byte of an array")
    ),
    "step-update": benchmark(
        "Update every Nth byte of an array",
        "Performs an update of every Nth byte of an array with some defined step",
        lambda: step("update", "Update every Nth byte of an array")
    ),
}


print("Benchmarks (version: 05.05.2023)")
print("Written by: https://github.com/vadimgush")
print("Use \"--help\" to get list of available benchmarks")
print("NOTE: If you're running these benchmarks on a laptop, make sure that laptop is connected to power adapter.\n"
      "Otherwise, CPU might prioritise efficiency cores which are less powerful and have less cache.\n")

if "--help" in sys.argv:
    print("python benchmark.py [benchmark name]...\n")
    print("Available benchmarks:")
    for key in benchmarks:
        bench = benchmarks[key]
        print(" * " + key + ": " + bench.description)
    print()
    exit(0)

benchmarks_to_run = []
for key in benchmarks:
    if key in sys.argv:
        benchmarks_to_run.append(benchmarks[key])

if len(benchmarks_to_run) == 0:
    print("-> No benchmarks were specified. Running all of them.")
    benchmarks_to_run = benchmarks.values()

build()
for bench in benchmarks_to_run:
    print("-> Benchmark: " + bench.name)
    bench.call()

print("-> All benchmarks completed!\n")
print("You can find benchmarks results in \'" + results_directory + "\' directory\n")
