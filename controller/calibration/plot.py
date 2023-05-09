import numpy as np
from mpl_toolkits import mplot3d
import matplotlib.pyplot as plt


rawData = np.genfromtxt('accDFR.txt',
                        delimiter=' ')

N = len(rawData)
print(N)

fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')
for i in range(N):
    xraw = rawData[i, 0]
    yraw = rawData[i, 1]
    zraw = rawData[i, 2]

    ax.scatter(xraw, yraw, zraw, color='r')

plt.show()