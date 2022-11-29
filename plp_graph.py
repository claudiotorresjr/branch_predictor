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
X_axis = np.arange(len(progs))
width = 0.25

# bht_1 = [78.3209, 78.7718, 94.3635, 89.3578, 95.4696, 94.4473]
# plt.bar(X_axis, bht_1, width=width, color='#22fb4f', label='BTB with initial BHT 1')

axL = plt.subplot()
plp = [81.2090, 78.7221, 94.8217, 90.2585, 96.2408, 94.0673]
plt.bar(X_axis, plp, width=width, color='#22fb4f', label='history size 1')

plp_2 = [84.0367, 91.6165, 95.8408, 91.3686, 96.2408, 94.7441]
plt.bar(X_axis+width, plp_2, width=width, color='#f86c67', label='history size 2')

plp_3 = [93.9231, 94.4501, 97.9038, 94.9524, 97.0119, 97.5962]
plt.bar(X_axis+width*2, plp_3, width=width, color='#a962f8', label='history size 8')

plt.ylabel("Percentage of right prediction")
plt.legend(loc='lower left')

plt.xticks(X_axis+(width*2)/2, progs)


# axR = axL.twinx()
# axR.yaxis.tick_right()
# axR.yaxis.set_label_position("right")

# time_btb = [23, 17, 25, 21, 19, 22]
# axR.scatter(np.arange(len(X_axis))+(width*2)/2, time_btb, color='#F8D0B8')
# axR.plot(np.arange(len(X_axis))+(width*2)/2, time_btb, color='#F8D0B8', label="History size 1")

# time_plp2 = [23, 16, 24, 20, 20, 22]
# axR.scatter(np.arange(len(time_plp2))+(width*2)/2, time_plp2, color='#6D6D6D')
# axR.plot(np.arange(len(time_plp2))+(width*2)/2, time_plp2, color='#6D6D6D', label="History size 2")

# time_plp3 = [23, 17, 24, 20, 19, 24]
# axR.scatter(np.arange(len(time_plp3))+(width*2)/2, time_plp3, color='#99CEF2')
# axR.plot(np.arange(len(time_plp3))+(width*2)/2, time_plp3, color='#99CEF2', label="History size 8")

# plt.ylabel("Time in seconds")


plt.grid(color='#95a5a6', linestyle='--', linewidth=1, axis='y', alpha=0.7)

plt.ticklabel_format(style='plain', useOffset=False, axis='y')
plt.xlabel("Traces")

plt.title(f"Comparison of branch predictors")
plt.legend(loc='lower right')
plt.tight_layout()
plt.style.use('seaborn')

plt.savefig("results/btb/save.png")