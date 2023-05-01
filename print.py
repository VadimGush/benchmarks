import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

df = pd.read_csv("result.csv", comment='#')

x = df['step']
y = df['time']

fig, ax = plt.subplots()

ax.plot(np.asarray(x, float), y)
ax.set_xlabel("step")
ax.set_ylabel("time")
ax.grid(True)

plt.show()

