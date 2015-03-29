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
from matplotlib.ticker import MultipleLocator, FormatStrFormatter
import math

params = {'legend.linewidth': 10}
plt.rcParams.update(params)
matplotlib.rcParams['pdf.fonttype'] = 42
matplotlib.rcParams['ps.fonttype'] = 42
ga_gens = {1:500, 2:3600, 3:8500, 4:1500 }
es_gens = {1:480, 2:3850, 3:8400, 4:1540 }
ss = [7056, 50625,126025,20736]
evals = [[i for i in range(50, int(ss[0]/2), int(0.025*ss[0]/2))], 
         [i for i in range(100, int(ss[1]/2), int(0.025*ss[1]/2))],
         [i for i in range(200, int(ss[2]/2), int(0.025*ss[2]/2))],
         [i for i in range(150, int(ss[3]/2), int(0.025*ss[3]/2))]]
def main():
    algo = ['es', 'ga', 'sa', 'hc']
    h_star = [0.498641, 0.496877, 0.49747, 0.49926]
    for tc in xrange(1, 5, 1):
        x = []
        y = []
        for a in algo:
            a_mf = []
            l_f = []
            for e in evals[tc-1]:
                fitness = []
                for r in range(10):
                    if a == 'ga':
                        i = int(e/ga_gens[tc]) 
                        l = e - i * ga_gens[tc]
                        fname = "tc%d/tc%d_%s_r%d_o%d_pop.csv" % (tc,tc,a, r, i)
                            #df = pd.read_csv(fname, header=None, index_col=False)
                            #fitness += df.sort([0]).iloc[0,0]
                            #count += 1
                    elif a == 'es':
                        i = int(e/es_gens[tc]) 
                        l = e - i * es_gens[tc]
                        fname = "tc%d/tc%d_%s_r%d_o%d_pop.csv" % (tc,tc,a, r, i)
                    else:
                        fname = "tc%d/tc%d_%s_r%d_iters.csv" % (tc,tc,a, r)
                        l = e
                    if os.path.isfile(fname):
                        df = pd.read_csv(fname, header=None, index_col=False)
                        if a in ['hc', 'sa']:
                            fitness.append(df[df[0] <= e].sort([1]).iloc[0,1])
                            #print fname, a, l, fitness[-1]
                        else:
                            fitness.append(df.sort([0]).iloc[0,0])
                    # if file doesn't exist, optimal found
                    elif len(fitness):
                        fitness.append(fitness[-1])
                if not len(fitness): 
                    a_mf.append(a_mf[-1])
                else:
                    if len(a_mf) and a_mf[-1] < np.sum(fitness)/len(fitness):
                        print 'warning', a
                        for i in [i for i in range(len(l_f)) if l_f[i] < fitness[i] ]:
                            fitness[i] = l_f[i]
                    a_mf.append(np.sum(fitness)/len(fitness))
                    print tc, a, sum(fitness)/len(fitness)
                l_f = fitness 
            y.append(a_mf)
            e = map(lambda x: 100*x/ss[tc-1], evals[tc-1]) 
            x.append(e)
        mx = max(map(max, y))
        mn = min(map(min, y))
        yt = np.arange(mn-0.0015, mx+0.001, 0.003)
        yt = np.insert(yt, 1, h_star[tc-1])
        ax = plt.subplot()
        plt.gca().xaxis.grid(True)
        plt.gca().yaxis.grid(True)
        ax.set_yticks(yt)
        #ax.set_xticks(e)
        #ax.xaxis.set_major_formatter(FormatStrFormatter('%d\%'))
        ax.set_ylim((mn-0.002,mx+0.001))
        for label in (ax.get_xticklabels() + ax.get_yticklabels()):
            label.set_fontname('Arial')
            label.set_fontsize(11)
        ax.get_yticklabels()[1].set_color('m')
        ax.plot(x[0],y[0],'b-D')
        ax.plot(x[1],y[1],'r-s')
        ax.plot(x[2],y[2],'g-*')
        ax.plot(x[3],y[3],'c-v')
        #plt.yscale('symlog')
        plt.legend(['ES','GA','SA','HC'], loc=0)
        plt.xlabel('Fitness Evaluations(%)', fontsize=13)
        plt.ylabel('Mean Best Fitness', fontsize=13)
        plt.axhline(y=h_star[tc-1], linestyle='dashed', linewidth=1, color='m')
        #plt.title('Test Case %d' % tc, fontsize=16)
        plt.savefig('/home/ajauhri/quals/paper/FIG/tc%d_mf.eps' % tc, format='eps', dpi=1000)
        #plt.show()
        plt.clf()

if __name__ == "__main__":
    main()
