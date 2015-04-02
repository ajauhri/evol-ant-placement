import sys
import numpy as np
import csv
import operator
from matplotlib import cm
from mpl_toolkits.mplot3d import axes3d
import matplotlib.pyplot as plt
import math
import scipy.interpolate
def tc1(fname, ant):
    res = np.loadtxt(open(fname, 'r'), delimiter=',', skiprows=1, usecols=range(9))
    b = np.ascontiguousarray(res[:,ant[0]:ant[0]+3]).view(np.dtype((np.void, res.dtype.itemsize * 3)))
    _, a1_idx = np.unique(b, return_index=True)
    b = np.ascontiguousarray(res[:,ant[1]:ant[1]+3]).view(np.dtype((np.void, res.dtype.itemsize * 3)))
    _, a2_idx = np.unique(b, return_index=True)
    a1_uniq_count = len(a1_idx) 
    a2_uniq_count = len(a2_idx)
    sorted_idx = np.lexsort((res[:,ant[0]], res[:,ant[0]+1], res[:,ant[0]+2], res[:,ant[1]], res[:,ant[1]+1], res[:,ant[1]+2]))
    a1 = []
    a2 = []
    f = []
    for i in range(a1_uniq_count):
        prev = np.array([0])
        for j in range(a2_uniq_count):
            ind = i*a1_uniq_count + j
            f.append(res[sorted_idx[ind], 0])
            a1.append(j)
            a2.append(i)
            if j>0:
                assert res[sorted_idx[ind], ant[1]:ant[1]+3].all() == prev.all()
            prev = res[sorted_idx[ind], ant[1]:ant[1]+3]
    f = np.array(f)
    a1 = np.array(a1)
    a2 = np.array(a2)
    fig = plt.figure()
    ax = fig.gca(projection='3d')
    ax.set_xlabel("Allowable Placements of Antenna 1", fontsize=8)
    ax.set_ylabel("Allowable Placements of Antenna 2", fontsize=8)
    ax.set_zlabel("Fitness", fontsize=8)
    surf = ax.plot_trisurf(a1, a2, f, cmap=cm.brg, linewidth=0.01)
    cbar = fig.colorbar(surf)
    #cbar.set_label('\n\n\n\n\nFitness scale', rotation=270)
    ax.view_init(elev=70., azim=-57)
    plt.savefig('/home/ajauhri/quals/paper/FIG/tc1_ss.png', format='png')#, dpi=1000)
    #plt.show()
    plt.clf()
    xi, yi = np.linspace(a1.min(), a2.max(), 100), np.linspace(a2.min(), a1.max(), 100)
    xi, yi = np.meshgrid(xi, yi)

    #Interpolate
    rbf = scipy.interpolate.Rbf(a1, a2, f, function='linear')
    zi = rbf(xi, yi)

    plt.imshow(zi, vmin=f.min(), vmax=f.max(), origin='lower',
           extent=[a1.min(), a1.max(), a2.min(), a2.max()])
    plt.colorbar()
    plt.show()
    plt.clf()

def tc2(fname, ant):
    res = np.loadtxt(open(fname, 'r'), delimiter=',', skiprows=1, usecols=range(12))
    b = np.ascontiguousarray(res[:,ant[0]:ant[0]+3]).view(np.dtype((np.void, res.dtype.itemsize * 3)))
    _, a1_idx = np.unique(b, return_index=True)
    b = np.ascontiguousarray(res[:,ant[1]:ant[1]+3]).view(np.dtype((np.void, res.dtype.itemsize * 3)))
    _, a2_idx = np.unique(b, return_index=True)
    b = np.ascontiguousarray(res[:,ant[2]:ant[2]+3]).view(np.dtype((np.void, res.dtype.itemsize * 3)))
    _, a3_idx = np.unique(b, return_index=True)

    a1_uniq_count = len(a1_idx) 
    a2_uniq_count = len(a2_idx)
    a3_uniq_count = len(a3_idx)
    sorted_idx = np.lexsort((res[:,ant[0]], res[:,ant[0]+1], res[:,ant[0]+2], res[:,ant[1]], res[:,ant[1]+1], res[:,ant[1]+2], res[:,ant[2]], res[:,ant[2]+1], res[:,ant[2]+2]))
    a12 = []
    a3 = []
    f = []
    for i in range(a3_uniq_count):
        for j in range(a2_uniq_count):
            for k in range(a1_uniq_count):
                ind = i*a2_uniq_count*a1_uniq_count + j*a1_uniq_count + k 
                f.append(res[sorted_idx[ind], 0])
                a12.append(k + j*a1_uniq_count)
                a3.append(i)
    f = np.array(f)
    a12 = np.array(a12)
    a3 = np.array(a3)
    fig = plt.figure()
    ax = fig.gca(projection='3d')
    #ax.set_ylabel("Allowable placements for Ant #2")
    #ax.set_zlabel("Fitness")
    surf = ax.plot_trisurf(a12, a3, f, cmap=cm.brg, linewidth=0.01)
    fig.colorbar(surf)
    plt.show()
    plt.clf()

def tc4(fname, ant):
    res = np.loadtxt(open(fname, 'r'), delimiter=',', skiprows=1, usecols=range(15))
    b = np.ascontiguousarray(res[:,ant[0]:ant[0]+3]).view(np.dtype((np.void, res.dtype.itemsize * 3)))
    _, a1_idx = np.unique(b, return_index=True)
    b = np.ascontiguousarray(res[:,ant[1]:ant[1]+3]).view(np.dtype((np.void, res.dtype.itemsize * 3)))
    _, a2_idx = np.unique(b, return_index=True)
    b = np.ascontiguousarray(res[:,ant[2]:ant[2]+3]).view(np.dtype((np.void, res.dtype.itemsize * 3)))
    _, a3_idx = np.unique(b, return_index=True)
    b = np.ascontiguousarray(res[:,ant[3]:ant[3]+3]).view(np.dtype((np.void, res.dtype.itemsize * 3)))
    _, a4_idx = np.unique(b, return_index=True)

    a1_uniq_count = len(a1_idx) 
    a2_uniq_count = len(a2_idx)
    a3_uniq_count = len(a3_idx)
    a4_uniq_count = len(a4_idx)
    sorted_idx = np.lexsort((res[:,ant[0]], res[:,ant[0]+1], res[:,ant[0]+2], res[:,ant[1]], res[:,ant[1]+1], res[:,ant[1]+2], res[:,ant[2]], res[:,ant[2]+1], res[:,ant[2]+2], res[:,ant[3]], res[:,ant[3]+1], res[:,ant[3]+2]))
    a123 = []
    a4 = []
    f = []
    for h in range(a4_uniq_count):
        for i in range(a3_uniq_count):
            for j in range(a2_uniq_count):
                for k in range(a1_uniq_count):
                    ind = h*a3_uniq_count*a2_uniq_count*a1_uniq_count + i*a2_uniq_count*a1_uniq_count + j*a1_uniq_count + k 
                    f.append(res[sorted_idx[ind], 0])
                    a123.append(k + j*a1_uniq_count )
                    a4.append(h + i*a2_uniq_count)
                    #print ind,k + j*a1_uniq_count + i*a2_uniq_count*a1_uniq_count,h,res[sorted_idx[ind], 3:6], res[sorted_idx[ind],6:9], res[sorted_idx[ind], 9:12], res[sorted_idx[ind], 12:15]
    f = np.array(f)
    a123 = np.array(a123)
    a4 = np.array(a4)
    fig = plt.figure()
    ax = fig.gca(projection='3d')
    surf = ax.plot_trisurf(a123, a4, f, cmap=cm.brg, linewidth=0.01)
    fig.colorbar(surf)
    plt.show()
    plt.clf()
    # search space plot
    xi, yi = np.linspace(a123.min(), a4.max(), 100), np.linspace(a4.min(), a123.max(), 100)
    xi, yi = np.meshgrid(xi, yi)

    #Interpolate
    rbf = scipy.interpolate.Rbf(a123, a4, f, function='linear')
    zi = rbf(xi, yi)

    plt.imshow(zi, vmin=f.min(), vmax=f.max(), origin='lower',
           extent=[a123.min(), a123.max(), a4.min(), a4.max()])
    plt.colorbar()
    plt.show()


#tc1("tc1_ex.csv", [3,6])
#tc2("tc2_ex.csv", [3,6,9])
#tc2("tc3_ex.csv", [3,6,9])
tc4("tc4_ex.csv", [3,6,9,12])
#plt.show()

