#! /usr/bin/env python
from __future__ import division
import sys
from optparse import OptionParser
import os.path
import pandas as pd
# for plotting
import numpy as np
from mpl_toolkits.mplot3d import axes3d
import matplotlib.pyplot as plt
import matplotlib
from scipy.interpolate import spline
from pylab import boxplot

params = {'legend.linewidth': 10}
plt.rcParams.update(params)
matplotlib.rcParams['pdf.fonttype'] = 42
matplotlib.rcParams['ps.fonttype'] = 42
gens = {'ga': {1:500, 2:3600, 3:8500, 4:1500 }, 'es': {1:480, 2:3850, 3:8400, 4:1540 }}
ss = [7056, 50625,126025,20736]
evals = [[i for i in range(50, int(ss[0]/2), int(0.025*ss[0]/2))], 
         [i for i in range(100, int(ss[1]/2), int(0.025*ss[1]/2))],
         [i for i in range(200, int(ss[2]/2), int(0.025*ss[2]/2))],
         [i for i in range(150, int(ss[3]/2), int(0.025*ss[3]/2))]]

e = 0.00001
def main():
    algo = ['es', 'ga', 'sa', 'hc']
    h_star = [0.498641, 0.496877, 0.49747, 0.49926]
    results = {'es': [], 'ga':[],'sa':[],'hc':[]}
    best_fitness = {'es': [], 'ga':[],'sa':[],'hc':[]}
    mean_fitness = {'es': [], 'ga':[],'sa':[],'hc':[]}
    std_fitness = {'es': [], 'ga':[],'sa':[],'hc':[]}
    for tc in xrange(1, 5, 1):
        for a in algo:
            a_p = [0]
            a_e = [0]
            count = 0
            runs = 0
            best_fitness[a].append([])
            for r in range(10):
                if a in ['es', 'ga']:
                    i = 0
                    while True:
                        fname = "tc%d/tc%d_%s_r%d_o%d_pop.csv" % (tc,tc,a, r, i)
                        if gens[a][tc]*i < evals[tc-1][-1] and os.path.isfile(fname):
                            with open(fname) as f:
                                fitness = float(f.readline().split(',')[0])
                                if abs(fitness - h_star[tc-1]) <= e:
                                    best_fitness[a][-1].append(fitness)
                                    count += 1
                                    runs += 1
                                    break
                                i += 1
                                if gens[a][tc]*i >= evals[tc-1][-1]:
                                    best_fitness[a][-1].append(fitness)
                                    runs += 1
                                    break
                else:
                    fname = "tc%d/tc%d_%s_r%d_iters.csv" % (tc,tc,a, r)
                    if os.path.isfile(fname):
                        runs += 1
                        df = pd.read_csv(fname, header=None, index_col=False)
                        fitness = df[df[0] <= evals[tc-1][-1]].sort([1]).iloc[0,1]
                        best_fitness[a][-1].append(fitness)
                        if abs(fitness - h_star[tc-1]) <= e:
                                count += 1
            results[a].append(count*100/runs)
            mean_fitness[a].append(np.mean(best_fitness[a][-1]))
            std_fitness[a].append(np.std(best_fitness[a][-1]))
            assert (len(best_fitness[a][-1]) == 5 or len(best_fitness[a][-1]) == 10)
    # plot success rates
    width = 0.15
    fig = plt.figure()
    ax = fig.add_subplot(111)
    ind = np.arange(4)
    #c = ['red','black','blue','yellow']
    c = ['blue','red','green','cyan']
    #patterns = ['.', '+', 'x', '-']
    cc = ['magenta', 'cyan', 'red','black']
    for i in xrange(len(algo)):
        ax.bar(ind+(i*width), results[algo[i]], width, color=c[i])
    ax.set_xlim(-width,len(ind)+width)
    ax.set_ylim(0,110)
    ax.yaxis.grid()
    ax.set_ylabel('Success Rates in %')
    plt.legend(['ES','GA','SA','HC'], bbox_to_anchor=(1.05,1))
    xticks = ['Test case ' + str(i) for i in range(1,5)]
    ax.set_xticks(ind+width)
    xtickNames = ax.set_xticklabels(xticks)
    plt.setp(xtickNames, rotation=30, fontsize=9)
    plt.savefig('/home/ajauhri/quals/paper/FIG/tc_sp.eps', format='eps', dpi=1000)
    plt.clf()
    
    # plot mean fitness with std dev.
    fig = plt.figure()
    ax = fig.add_subplot(111)
    for i in xrange(len(algo)):
        ax.bar(ind+(i*width), mean_fitness[algo[i]], width, color=c[i], yerr=std_fitness[algo[i]], error_kw=dict(elinewidth=2, ecolor=cc[i]))
    ax.set_xlim(-width,len(ind)+width)
    ax.set_ylim(0.44,0.56)
    ax.yaxis.grid()
    ax.set_ylabel('Mean Best Fitness')
    plt.legend(['ES','GA','SA','HC'], loc='upper right')
    xticks = ['Test case ' + str(i) for i in range(1,5)]
    ax.set_xticks(ind+width)
    xtickNames = ax.set_xticklabels(xticks)
    plt.setp(xtickNames, rotation=30, fontsize=9)
    plt.savefig('/home/ajauhri/quals/paper/FIG/tc_mfwerr.eps', format='eps', dpi=1000)
    plt.show() 

if __name__ == "__main__":
    main()
