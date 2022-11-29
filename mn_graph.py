import numpy as np
import pandas as pd
from pandas import Series, DataFrame
import matplotlib.pyplot as plt
from matplotlib.ticker import FuncFormatter


tam = [
    "1x65536",
    "2x32768",
    "4x16384",
    "8x8192",
    "16x4096",
    "32x2048",
    "64x1024",
    "128x512",
    "256x256",
    "512x128",
    "1024x64",
    "2048x32",
    "4096x16",
    "8192x8",
    "16384x4",
    "32768x2",
    "65536x1"
]
X_axis = np.arange(len(tam))
width = 0.25

# astar = [
#     97.2252, 97.5412, 97.5802, 97.5893, 97.7311, 97.7333, 97.7334, 97.7334, 97.7346, 97.7345, 97.7355, 97.7310, 97.7270, 97.6993, 97.5996, 97.3328, 96.7483
#     ]
# # plt.scatter(np.arange(len(X_axis))+(width*2)/2, astar)
# plt.plot(np.arange(len(X_axis))+(width*6)/2, astar, label="astar")
# # plt.bar(X_axis, plp, width=width, color='#22fb4f', label='history size 1')

# calculix = [
#     98.7094, 98.7343, 98.7415, 98.7458, 98.7478, 98.7488, 98.7497, 98.7501, 98.7495, 98.7505, 98.7515, 98.7513, 98.7495, 98.7478, 98.7483, 98.7442, 98.7286
#     ]
# # plt.scatter(np.arange(len(X_axis))+(width*2)/2, calculix,)
# plt.plot(np.arange(len(X_axis))+(width*6)/2, calculix, label="calculix")

# dealII = [
#     99.3528, 99.3864, 99.4110, 99.4189, 99.4302, 99.4391, 99.4423, 99.4454, 99.4435, 99.4490, 99.4505, 99.4491, 99.4494, 99.4482, 99.4454, 99.4412, 99.4224
# ]
# # plt.scatter(np.arange(len(X_axis))+(width*2)/2, dealII)
# plt.plot(np.arange(len(X_axis))+(width*6)/2, dealII, label="dealII")

# gromacs = [
#     96.1093, 96.2795, 96.3770, 96.4140, 96.4478, 96.4632, 96.4767, 96.5244, 96.5210, 96.5277, 96.5083, 96.4946, 96.4837, 96.4682, 96.4209, 96.2832, 95.9513
# ]
# # plt.scatter(np.arange(len(X_axis))+(width*2)/2, gromacs)
# plt.plot(np.arange(len(X_axis))+(width*6)/2, gromacs, label="gromacs")

# libquantum = [
#     99.7572, 99.7570, 99.7569, 99.7570, 99.7570, 99.7570, 99.7570, 99.7570, 99.7570, 99.7570, 99.7570, 99.7570, 99.7570, 99.7571, 99.7570, 99.7567, 99.7570
# ]
# # plt.scatter(np.arange(len(X_axis))+(width*2)/2, libquantum)
# plt.plot(np.arange(len(X_axis))+(width*6)/2, libquantum, label="libquantum")

# namd = [
#     99.2586, 99.2623, 99.2653, 99.2674, 99.2688, 99.2694, 99.2692, 99.2703, 99.2694, 99.2693, 99.2699, 99.2699, 99.2698, 99.2692, 99.2686, 99.2686, 99.2671
# ]
# # plt.scatter(np.arange(len(X_axis))+(width*2)/2, namd)
# plt.plot(np.arange(len(X_axis))+(width*6)/2, namd, label="namd")







# astar = [
#     91.5257,92.5894,92.6577,92.6583,92.6706,92.6715,92.6704,92.6721,92.6719,92.6719,92.6717,92.6744,92.6769,92.7058,92.6955,92.4953,90.0300
#     ]
# # plt.scatter(np.arange(len(X_axis))+(width*2)/2, astar)
# plt.plot(np.arange(len(X_axis))+(width*6)/2, astar, label="astar")
# # plt.bar(X_axis, plp, width=width, color='#22fb4f', label='history size 1')

# calculix = [
#     92.4598,92.8754,92.9289,92.9315,92.9329,92.9345,92.9359,92.9359,92.9366,92.9368,92.9347,92.9348,92.9378,92.9357,92.9359,92.9349,92.1689
# ]
# # plt.scatter(np.arange(len(X_axis))+(width*2)/2, calculix,)
# plt.plot(np.arange(len(X_axis))+(width*6)/2, calculix, label="calculix")

# dealII = [
#     96.8763,97.0584,97.2327,97.2361,97.1551,97.2443,97.2498,97.2260,97.2534,97.2545,97.2536,97.2547,97.2534,97.2523,97.2433,97.1877,96.9660
# ]
# # plt.scatter(np.arange(len(X_axis))+(width*2)/2, dealII)
# plt.plot(np.arange(len(X_axis))+(width*6)/2, dealII, label="dealII")

# gromacs = [
#     94.5117,94.5421,94.5604,94.5698,94.5654,94.5685,94.5701,94.5703,94.5833,94.5673,94.5725,94.5693,94.3013,94.2893,94.0844,94.0102,93.0904
# ]
# # plt.scatter(np.arange(len(X_axis))+(width*2)/2, gromacs)
# plt.plot(np.arange(len(X_axis))+(width*6)/2, gromacs, label="gromacs")

# libquantum = [
#     96.6692,96.6263,96.6264,96.6264,96.6264,96.6264,96.6264,96.6264,96.6264,96.6264,96.6264,96.6264,96.6264,96.6264,96.6264,96.6264,96.6264
# ]
# # plt.scatter(np.arange(len(X_axis))+(width*2)/2, libquantum)
# plt.plot(np.arange(len(X_axis))+(width*6)/2, libquantum, label="libquantum")

# namd = [
#     97.1385,97.1482,97.2471,97.2888,97.2906,97.2849,97.2817,97.2911,97.2371,97.2922,97.2924,97.0791,97.0794,97.0782,97.0786,97.0786,96.6892
# ]
# # plt.scatter(np.arange(len(X_axis))+(width*2)/2, namd)
# plt.plot(np.arange(len(X_axis))+(width*6)/2, namd, label="namd")


# astar = [
#     97.2252, 97.5412, 97.5802, 97.5893, 97.7311, 97.7333, 97.7334, 97.7334, 97.7346, 97.7345, 97.7355, 97.7310, 97.7270, 97.6993, 97.5996, 97.3328, 96.7483
#     ]
# plt.plot(np.arange(len(X_axis))+(width*6)/2, astar, label="astar")


axL = plt.subplot()
astar_time = [
    23,22,23,22,23,23,23,23,23,22,23,22,23,22,23,23,22
]
plt.bar(np.arange(len(X_axis))+(width*2)/2, astar_time, color='#a962f8', label="astar_time")
plt.ylabel("Time in seconds")
plt.legend(loc='lower right')

plt.xticks(X_axis+width, tam, rotation=45)

axR = axL.twinx()
axR.yaxis.tick_right()
axR.yaxis.set_label_position("right")

astar = [
    91.5257,92.5894,92.6577,92.6583,92.6706,92.6715,92.6704,92.6721,92.6719,92.6719,92.6717,92.6744,92.6769,92.7058,92.6955,92.4953,90.0300
    ]
axR.plot(np.arange(len(X_axis)), astar, linewidth=2, color="red", label="astar")

plt.ylabel("Percentage of right prediction")
plt.legend(loc='lower left')


plt.grid(color='#95a5a6', linestyle='--', linewidth=1, axis='y', alpha=0.7)
plt.grid(color='#95a5a6', linestyle='--', linewidth=1, axis='x', alpha=0.7)

plt.ticklabel_format(style='plain', useOffset=False, axis='y')
plt.xticks(X_axis+width, tam, rotation=45)
plt.xlabel("W matrix dimension")

plt.title(f"Comparison of different W matrix size with history size = 5")
plt.tight_layout()
plt.style.use('seaborn')

plt.savefig("save.png")