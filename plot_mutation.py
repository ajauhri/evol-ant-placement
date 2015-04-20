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
    f.close()

def plot2(id):
    fig = plt.figure()
    ax = fig.add_subplot(111, projection='3d')
    f = file("luas/tc%d_ex.lua" % id, 'r')
    lines = f.readlines()
    plt_f = re.findall("\"([^\"]+)\"", lines[0])
    plot_platform(plt_f[0], ax, id)
    colors = ['r','g','b','y',]
    x_p = []
    y_p = []
    z_p = []
    ant_count = 0
    for line in lines[1:]:
        if "add_antenna" in line:
            if ant_count == 1:
                break
            ant_count += 1
        elif "add_point" in line:
            p = re.findall("\(([^\"]+)\)", line)
            p = map(float, p[0].split(','))
            x_p.append(p[0])
            y_p.append(p[1])
            z_p.append(p[2])
    assert len(x_p) == len(y_p) == len(z_p)
    ax.plot(x_p, y_p, z_p, "o", color = colors[0], label="Allowable placements for antenna B")
    ax.plot([x_p[47]], [y_p[47]], [z_p[47]], "*", markersize=9, color = colors[2], label="New placement for antenna B")
    f.close()
    ax.set_xlabel("x")
    ax.set_ylabel("y")
    ax.set_zlabel("z")
    #ax.set_title("Test Case %d" % id)
    labels = map(lambda x: "Antenna %d" % x, range(1,2))
    leg = ax.legend(bbox_to_anchor=(1,0.72), numpoints=1, prop={'size':9})
    leg.get_frame().set_linewidth(0.0)
    plt.savefig('/home/ajauhri/quals/paper/FIG/tc%d_mut2.eps' % id, format='eps', dpi=1000)
    plt.show()
    plt.clf()
    return ([x_p[47]], [y_p[47]], [z_p[47]])

def plot1(id):
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
    ax.plot(x_p[1], y_p[1], z_p[1], "o", markersize=9, color = colors[1], label="Placement for antenna A")
    ax.plot(x_p[0], y_p[0], z_p[0], "o", markersize=9,color = colors[0], label="Placement for antenna B")
    f.close()
    ax.set_xlabel("x")
    ax.set_ylabel("y")
    ax.set_zlabel("z")
    #ax.set_title("Test Case %d" % id)
    labels = map(lambda x: "Antenna %d" % x, range(1,ant[tc_id-1]+1))
    leg = ax.legend(bbox_to_anchor=(1,0.72), numpoints=1, prop={'size':9})
    leg.get_frame().set_linewidth(0.0)
    plt.savefig('/home/ajauhri/quals/paper/FIG/tc%d_mut1.eps' % tc_id, format='eps', dpi=1000)
    plt.show()
    plt.clf()
    return (x_p[1], y_p[1], z_p[1])

def plot3(new, orig):
    colors = ['r','g','b','y',]
    fig = plt.figure()
    ax = fig.add_subplot(111, projection='3d')
    f = file("luas/tc1_ex.lua", 'r')
    lines = f.readlines()
    plt_f = re.findall("\"([^\"]+)\"", lines[0])
    plot_platform(plt_f[0], ax, id)
    f.close()
    plot_platform(plt_f[0], ax, 1)
    ax.plot(orig[0], orig[1], orig[2], "o", markersize=9, color = colors[1], label="Placement for antenna A")
    ax.plot(new[0], new[1], new[2], "o", markersize=9, color = colors[2], label="Placement for antenna B")
    ax.set_xlabel("x")
    ax.set_ylabel("y")
    ax.set_zlabel("z")
    #ax.set_title("Test Case %d" % id)
    labels = map(lambda x: "Antenna %d" % x, range(1,3))
    leg = ax.legend(bbox_to_anchor=(1,0.72), numpoints=1, prop={'size':9})
    leg.get_frame().set_linewidth(0.0)
    plt.savefig('/home/ajauhri/quals/paper/FIG/tc1_mut3.eps', format='eps', dpi=1000)
    plt.clf()

orig = plot1(11)
new = plot2(1)
plot3(new, orig)
