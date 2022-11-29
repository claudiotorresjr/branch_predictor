import numpy as np
import pandas as pd
from pandas import Series, DataFrame
import matplotlib.pyplot as plt
from matplotlib.ticker import FuncFormatter

# progs = ["astar", "calculix", "dealII", "gromacs", "libquantum", "namd"]
# X_axis = np.arange(len(progs))
# width = 0.25

# axL = plt.subplot()
# bht_1 = [78.3209, 78.7718, 94.3635, 89.3578, 95.4696, 94.4473]
# plt.bar(X_axis, bht_1, width=width, color='#22fb4f', label='BTB com BHT (1)')

# plp = [81.2090, 78.7221, 94.8217,90.2585, 96.2408, 94.0673]
# plt.bar(X_axis+width, plp, width=width, color='#a962f8', label='Piecewise Linear')

# plt.ylabel("Percentage of right prediction")
# plt.legend(bbox_to_anchor =(0,-0.27), loc='lower left')

# plt.xticks(X_axis+width/2, progs)


# axR = axL.twinx()
# axR.yaxis.tick_right()
# axR.yaxis.set_label_position("right")

# time_btb = [23, 17, 24, 21, 19, 22]
# axR.scatter(np.arange(len(X_axis))+width/2, time_btb, color='#0614ae')
# axR.plot(np.arange(len(X_axis))+width/2, time_btb, color='#0614ae', label="Tempo BTB")

# time_plp = [23, 18, 24, 21, 20, 23]
# axR.scatter(np.arange(len(time_plp))+width/2, time_plp, color='#f86c67')
# axR.plot(np.arange(len(time_plp))+width/2, time_plp, color='#f86c67', label="Tempo Piecewise")
# plt.ylabel("Time in seconds")


# plt.grid(color='#95a5a6', linestyle='--', linewidth=1, axis='y', alpha=0.7)

# plt.ticklabel_format(style='plain', useOffset=False, axis='y')
# plt.xlabel("Traces")

# plt.title(f"Comparison of branch predictors")
# plt.legend(bbox_to_anchor =(1,-0.27), loc='lower right')
# plt.tight_layout()
# plt.style.use('seaborn')

# plt.savefig("results/btb/save.png")

progs = ["astar", "calculix", "dealII", "gromacs", "libquantum", "namd"]
tam = [1, 10, 20, 30, 40, 50, 60, 70, 80, 90]
X_axis = np.arange(len(tam))
width = 0.25

# bht_1 = [78.3209, 78.7718, 94.3635, 89.3578, 95.4696, 94.4473]
# plt.bar(X_axis, bht_1, width=width, color='#22fb4f', label='BTB with initial BHT 1')

astar = [81.2090, 94.8362, 96.9321, 97.2164, 97.3537, 97.4660, 97.5790, 97.6355, 97.6545, 97.6606]
# plt.scatter(np.arange(len(X_axis))+(width*2)/2, astar)
plt.plot(np.arange(len(X_axis))+(width*6)/2, astar, label="astar")
# plt.bar(X_axis, plp, width=width, color='#22fb4f', label='history size 1')

calculix = [78.7221, 96.6953, 97.3948, 98.5300, 98.5494, 98.6943, 98.7473, 98.7632, 99.1523, 99.1569]
# plt.scatter(np.arange(len(X_axis))+(width*2)/2, calculix,)
plt.plot(np.arange(len(X_axis))+(width*6)/2, calculix, label="calculix")

dealII = [94.8217, 98.1983, 98.8220, 99.1309, 99.2852, 99.4168, 99.4359, 99.4588, 99.4670, 99.4683]
# plt.scatter(np.arange(len(X_axis))+(width*2)/2, dealII)
plt.plot(np.arange(len(X_axis))+(width*6)/2, dealII, label="dealII")

gromacs = [90.2585, 95.3962, 96.0691, 96.1426, 96.3459, 96.3880, 96.4056, 96.4192, 96.4250, 96.4360]
# plt.scatter(np.arange(len(X_axis))+(width*2)/2, gromacs)
plt.plot(np.arange(len(X_axis))+(width*6)/2, gromacs, label="gromacs")

libquantum = [96.2408, 97.0119, 97.6642, 98.4345, 99.5270, 99.5890, 99.6806, 99.8369, 99.9290, 99.9698]
# plt.scatter(np.arange(len(X_axis))+(width*2)/2, libquantum)
plt.plot(np.arange(len(X_axis))+(width*6)/2, libquantum, label="libquantum")

namd = [94.0673, 97.9304, 98.5532, 98.9656, 99.1684, 99.2150, 99.2670, 99.2711, 99.3338, 99.3556]
# plt.scatter(np.arange(len(X_axis))+(width*2)/2, namd)
plt.plot(np.arange(len(X_axis))+(width*6)/2, namd, label="namd")

plt.ylabel("Percentage of right prediction")
plt.legend(loc='lower left')

plt.xticks(X_axis+(width*6)/2, tam)

plt.grid(color='#95a5a6', linestyle='--', linewidth=1, axis='y', alpha=0.7)

plt.ticklabel_format(style='plain', useOffset=False, axis='y')
plt.xlabel("Size of branch history")

plt.title(f"Comparison of branch predictions with differents branch history size")
plt.legend(loc='lower right')
plt.tight_layout()
plt.style.use('seaborn')

plt.savefig("save.png")