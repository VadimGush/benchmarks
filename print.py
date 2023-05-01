import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

df = pd.read_csv("result.csv", comment='#')

x = df['step']
y = df['time']

fig, ax = plt.subplots()

ax.plot(np.asarray(x, float), y)
ax.set_xlabel("step (N)")
ax.set_ylabel("time (microseconds)")
ax.grid(True)
ax.set_title("Update every Nth byte of an array")

plt.show()

