import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import numpy as np
import sys
import re

def plot_platform(fname, ax, id):
    f = file(fname, 'r')
    start = []
    end = []
    for line in f.readlines():
        if "GW" in line:
            start = map(float, line.split()[3:6])
            end = map(float, line.split()[6:9])
            ax.plot([start[0], end[0]], [start[1], end[1]], [start[2], end[2]], color='black')
    ax.set_axis_off()
    plt.savefig('/home/ajauhri/quals/paper/FIG/tc%d_platform.eps' % id, format='eps', dpi=1000)
    f.close()

def main(id):
    fig = plt.figure()
    ax = fig.add_subplot(111, projection='3d')
    ant = [2,3,3,4]
    f = file("luas/tc%d_ex.lua" % id, 'r')
    lines = f.readlines()
    plt_f = re.findall("\"([^\"]+)\"", lines[0])
    plot_platform(plt_f[0], ax, id)
    colors = ['r','g','b','y',]
    curr_color = -1
    x_p = []
    y_p = []
    z_p = []
    for line in lines[1:]:
        if "add_antenna" in line:
            curr_color += 1
            x_p.append([])
            y_p.append([])
            z_p.append([])
        elif "add_point" in line:
            p = re.findall("\(([^\"]+)\)", line)
            p = map(float, p[0].split(','))
            x_p[curr_color].append(p[0])
            y_p[curr_color].append(p[1])
            z_p[curr_color].append(p[2])
    assert len(x_p) == len(y_p) == len(z_p)
    if id == 11:
        tc_id = 1
    else:
        tc_id = id
    for i in range(ant[tc_id-1]):
        ax.plot(x_p[i], y_p[i], z_p[i], "o", color = colors[i], label="Allowable placements for antenna %d"%(i+1))
    f.close()
    ax.set_xlabel("x")
    ax.set_ylabel("y")
    ax.set_zlabel("z")
    #ax.set_title("Test Case %d" % id)
    labels = map(lambda x: "Antenna %d" % x, range(1,ant[tc_id-1]+1))
    leg = ax.legend(bbox_to_anchor=(1,0.72), numpoints=1, prop={'size':9})
    leg.get_frame().set_linewidth(0.0)
    plt.savefig('/home/ajauhri/quals/paper/FIG/tc%d_intro.eps' % id, format='eps', dpi=1000)
    plt.show()
    plt.clf()
main(1)
main(11)
