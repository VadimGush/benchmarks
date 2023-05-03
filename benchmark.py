import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import os

def build():
    print("-> Compiling benchmarks...\n")
    # Build benchmark with required parameters
    os.system("cmake -DCMAKE_BUILD_TYPE=Release -S . -B build")
    os.system("cd build; make -j8")
    os.system("mkdir results")
    print("\n-> Compilation complete!")

def step(name, type, title):
    name = "step_" + type
    os.system("./build/" + name + " > ./build/" + name + ".csv")

    df = pd.read_csv("./build/" + name + ".csv", comment='#')
    x = df['step']
    y = df['time']
    fig, ax = plt.subplots()
    ax.plot(np.asarray(x, float), y)
    ax.set_xlabel("step (N)")
    ax.set_ylabel("time (microseconds)")
    ax.grid(True)
    ax.set_title(title)

    plt.savefig("results/" + name + ".png", dpi=300)

print("Benchmarks (version: 03.05.2023)")
print("Written by: https://github.com/vadimgush\n")
build()

print("-> Benchmark: Sum every Nth byte of an array")
step("step-sum", "sum", "Sum every Nth byte of an array")

print("-> Benchmark: Update every Nth byte of an array")
step("step-update", "update", "Update every Nth byte of an array")


