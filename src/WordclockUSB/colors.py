#! /usr/bin/env python3
# -*- coding: utf-8 -*-

import numpy as np
import matplotlib.pyplot as plt
from matplotlib.patches import Rectangle


def cvalue(index):
    index %= 102
    if index < 17:
        return index * 15 / 255
    elif index < 51:
        return 255 / 255
    elif index < 68:
        return (68 - index) * 15 / 255
    else:
        return 0


def ccode(index):
    return cvalue(index), cvalue(index + 34), cvalue(index + 68)


plt.style.use("dark_background")

r = np.zeros(17 * 6)
g = np.zeros(17 * 6)
b = np.zeros(17 * 6)

for index in range(len(r)):
    r[index], g[index], b[index] = ccode(index)

rects = [
    Rectangle((i - 0.5, -0.45), 1, 0.4, ec=None, fc=(r[i], g[i], b[i]))
    for i in range(len(r))
]

fig = plt.figure(111)
fig.clf()
ax = fig.add_subplot(111)

t = np.arange(len(r))
ax.plot(t, r, "r-")
ax.plot(t, g, "g-")
ax.plot(t, b, "b-")

for rect in rects:
    ax.add_artist(rect)

ax.set_ylim(-0.5, 1.05)

plt.show()
