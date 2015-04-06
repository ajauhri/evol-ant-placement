import sys
import numpy as np
import csv
import os
import operator
from matplotlib import cm
from mpl_toolkits.mplot3d import axes3d
import matplotlib.pyplot as plt
import math
import scipy.interpolate

def save_tc(x, y, f, fname):
    out = file(fname, 'w')
    for i in range(y.shape[0]):
        out.write("%d,%d,%.6f\n" % (x[i], y[i], f[i]))
    out.close()


def tc1(ant):
    res = np.loadtxt(open("tc1"+"_ex.csv", 'r'), delimiter=',', skiprows=1, usecols=range(9))
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
    save_tc(a1,a2,f,'tc1.csv')
    for r in range(10):
        rfname = "tc1/tc1_es_r2_o%d_pop.csv" % (r)
        if not os.path.isfile(rfname):
            break
        run = np.loadtxt(open(rfname, 'r'), delimiter=',', usecols=range(9))
        run_ind = []
        for i in run:
            ind = int(np.where((res[:,3:9] == i[3:9]).all(axis=1))[0])
            run_ind.append(ind)
        run_ind = np.unique(run_ind)
        run_plot = np.empty((len(run_ind),2))
        occurs = np.intersect1d(sorted_idx, run_ind)
        run_plot[:,0] = a1[occurs]
        run_plot[:,1] = a2[occurs]
        run_fd = open('run' + str(r), 'w')
        for i in range(len(run_plot)):
            run_fd.write("%d,%d\n" % (run_plot[i][0], run_plot[i][1]))
        run_fd.close()
    '''
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
    plt.clf()
    '''
 
def tc2(ant):
    res = np.loadtxt(open("tc2"+"_ex.csv", 'r'), delimiter=',', skiprows=1, usecols=range(12))
    b = np.ascontiguousarray(res[:,ant[0]:ant[0]+3]).view(np.dtype((np.void, res.dtype.itemsize * 3)))
    _, a1_idx = np.unique(b, return_index=True)
    b = np.ascontiguousarray(res[:,ant[1]:ant[1]+3]).view(np.dtype((np.void, res.dtype.itemsize * 3)))
    _, a2_idx = np.unique(b, return_index=True)
    b = np.ascontiguousarray(res[:,ant[2]:ant[2]+3]).view(np.dtype((np.void, res.dtype.itemsize * 3)))
    _, a3_idx = np.unique(b, return_index=True)

    a1_uniq_count = len(a1_idx) 
    a2_uniq_count = len(a2_idx)
    a3_uniq_count = len(a3_idx)
    sorted_idx = np.lexsort((res[:,ant[2]], res[:,ant[2]+1], res[:,ant[2]+2], res[:,ant[1]], res[:,ant[1]+1], res[:,ant[1]+2], res[:,ant[0]], res[:,ant[0]+1], res[:,ant[0]+2]))
    a23 = []
    a1 = []
    f = []
    for i in range(a1_uniq_count):
        for j in range(a2_uniq_count):
            for k in range(a3_uniq_count):
                ind = i*a2_uniq_count*a3_uniq_count + j*a3_uniq_count + k 
                f.append(res[sorted_idx[ind], 0])
                a23.append(k + j*a3_uniq_count)
                a1.append(i)
    f = np.array(f)
    a23 = np.array(a23)
    a1 = np.array(a1)
    save_tc(a23,a1,f,'tc2.csv')
    for r in range(10):
        rfname = "tc2/tc2_es_r1_o%d_pop.csv" % (r)
        if not os.path.isfile(rfname):
            break
        run = np.loadtxt(open(rfname, 'r'), delimiter=',', usecols=range(12))
        run_ind = []
        for i in run:
            ind = int(np.where((res[:,3:12] == i[3:12]).all(axis=1))[0])
            run_ind.append(ind)
        run_ind = np.unique(run_ind)
        run_plot = np.empty((len(run_ind),2))
        occurs = np.intersect1d(sorted_idx, run_ind)
        run_plot[:,0] = a23[occurs]
        run_plot[:,1] = a1[occurs]
        run_fd = open('run' + str(r), 'w')
        for i in range(len(run_plot)):
            run_fd.write("%d,%d\n" % (run_plot[i][0], run_plot[i][1]))
        run_fd.close()
   

def tc3(ant):
    res = np.loadtxt(open("tc3"+"_ex.csv", 'r'), delimiter=',', skiprows=1, usecols=range(12))
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
    save_tc(a12,a3,f,'tc3.csv')
    for r in range(10):
        rfname = "tc3/tc3_es_r2_o%d_pop.csv" % (r)
        if not os.path.isfile(rfname):
            break
        run = np.loadtxt(open(rfname, 'r'), delimiter=',', usecols=range(12))
        run_ind = []
        for i in run:
            ind = int(np.where((res[:,3:12] == i[3:12]).all(axis=1))[0])
            run_ind.append(ind)
        run_ind = np.unique(run_ind)
        run_plot = np.empty((len(run_ind),2))
        occurs = np.intersect1d(sorted_idx, run_ind)
        run_plot[:,0] = a12[occurs]
        run_plot[:,1] = a3[occurs]
        run_fd = open('run' + str(r), 'w')
        for i in range(len(run_plot)):
            run_fd.write("%d,%d\n" % (run_plot[i][0], run_plot[i][1]))
        run_fd.close()
   
def tc4(ant):
    res = np.loadtxt(open("tc4"+"_ex.csv", 'r'), delimiter=',', skiprows=1, usecols=range(15))
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
                    a123.append(k + j*a1_uniq_count + i*a2_uniq_count*a1_uniq_count)
                    #a4.append(h + i*a2_uniq_count)
                    a4.append(h)
    f = np.array(f)
    a123 = np.array(a123)
    a4 = np.array(a4)
    save_tc(a123, a4, f, 'tc4.csv')

    for r in range(10):
        rfname = "tc4/tc4_ga_r2_o%d_pop.csv" % (r)
        if not os.path.isfile(rfname):
            break
        run = np.loadtxt(open(rfname, 'r'), delimiter=',', usecols=range(15))
        run_ind = []
        for i in run:
            ind = int(np.where((res[:,3:15] == i[3:15]).all(axis=1))[0])
            run_ind.append(ind)
        run_ind = np.unique(run_ind)
        run_plot = np.empty((len(run_ind),2))
        occurs = np.intersect1d(sorted_idx, run_ind)
        run_plot[:,0] = a123[occurs]
        run_plot[:,1] = a4[occurs]
        run_fd = open('run' + str(r), 'w')
        for i in range(len(run_plot)):
            run_fd.write("%d,%d\n" % (run_plot[i][0], run_plot[i][1]))
        run_fd.close()

#tc1([3,6])
tc2([3,6,9])
#tc3([3,6,9])
#tc4([3,6,9,12])
#plt.show()
