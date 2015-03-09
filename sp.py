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

params = {'legend.linewidth': 10}
plt.rcParams.update(params)
matplotlib.rcParams['pdf.fonttype'] = 42
matplotlib.rcParams['ps.fonttype'] = 42
ga_gens = {1:500, 2:3600, 3:8500, 4:1500 }
es_gens = {1:480, 2:3850, 3:8400, 4:1540 }
e = 0.000009
def main():
    algo = ['es', 'ga', 'sa', 'hc']
    h_star = [0.498641, 0.496877, 0.49747, 0.49926]
    for tc in xrange(1, 5, 1):
        x = []
        y = []
        for a in algo:
            a_p = [0]
            a_e = [0]
            for i in range(8):
                count = 0
                for r in range(10):
                    if a in ['es', 'ga']:
                        fname = "tc%d/tc%d_%s_r%d_o%d_pop.csv" % (tc,tc,a, r, i)
                        if os.path.isfile(fname):
                            with open(fname) as f:
                                fitness = float(f.readline().split(',')[0])
                                if abs(fitness - h_star[tc-1]) <= e:
                                    count += 1
                        else:
                            count += 1  
                    else:
                        fname = "tc%d/tc%d_%s_r%d_iters.csv" % (tc,tc,a, r)
                        if os.path.isfile(fname):
                            df = pd.read_csv(fname, header=None, index_col=False)
                            evals = (i+1) * es_gens[tc]
                            if len(df[(df[0] <= evals) & (abs(df[1] - h_star[tc-1]) <= e)]) > 0:
                                count += 1
                        else:
                            count += 1

                #print a, tc, (i+1)*es_gens[tc], count 
                if i >= 1 and count/10 < a_p[-1]:
                    print 'warning'
                    a_p.append(a_p[-1])
                else:
                    a_p.append(count/10)
                a_e.append((i+1) * es_gens[tc])
            print a_e
            xnew = np.linspace(min(a_e), max(a_e), 30)
            a_p = spline(a_e, a_p, xnew, order=1)
            y.append(a_p)
            x.append(xnew)
        plt.plot(x[0],y[0],'b-D')#, markersize=8)
        plt.plot(x[1],y[1],'r-s')#, markersize=8)
        plt.plot(x[2],y[2],'g-*')#, markersize=8)
        plt.plot(x[3],y[3],'c-v')#, markersize=8)
        plt.legend(['AP-ES','AP-GA','AP-SA','AP-HC'], loc=0)
        plt.xlabel('Evaluations', fontsize=18)
        plt.ylabel('Success probability', fontsize=18)
        plt.savefig('tc%d_sp.eps' % tc, format='eps', dpi=1000)
        plt.ylim((0,1.02))
        plt.title('Test Case %d' % tc)
        plt.show()
        plt.clf()

if __name__ == "__main__":
    main()
