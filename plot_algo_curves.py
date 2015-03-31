#! /usr/bin/env python
import sys
import os

from optparse import OptionParser

# for plotting
import numpy as np
from mpl_toolkits.mplot3d import axes3d
import matplotlib.pyplot as plt
from matplotlib.ticker import MaxNLocator
import matplotlib

params = {'legend.linewidth': 10}
algs = ['ga', 'es', 'sa', 'hc']
ga_gens = {'1':500, '2':3600, '3':8500, '4':1500 }
es_gens = {'1':480, '2':3850, '3':8400, '4':1540 }
ga_runs = {'1':'0', '2':'3', '3':'1', '4':'4'}
es_runs = {'1':'0', '2':'3', '3':'2', '4':'1'}

plt.rcParams.update(params)
matplotlib.rcParams['pdf.fonttype'] = 42
matplotlib.rcParams['ps.fonttype'] = 42
matplotlib.rcParams.update({'font.size': 8})

# argv[1] = test case id 
# argv[2] = algorithm 
# argv[3] = antennas 
# argv[4] = runs 
# argv[5] = generations 
def main():
    parser = OptionParser()
    parser.add_option("-i", "--id", dest="tc_id", help="tc id")
    
    (options, args) = parser.parse_args()
    if (not options.tc_id):
        parser.error("All options not given")
    x = list()
    y = list()
    for i in ['ga', 'es', 'sa', 'hc']:
        x_i = []
        y_i = []
        folder = "tc" + options.tc_id
        if i == "ga":
            for o in xrange(0, 10):
                file = folder + "/" + folder + "_" + i + "_r" +ga_runs[options.tc_id]+ "_o" + str(o) + "_pop.csv"
                if os.path.exists(file):
                    f = open(file, 'r')
                    line = f.readline()
                    fitness = float(line.split(",")[0])
                    
                    x_i.append((o+1))
                    y_i.append(fitness)
                else:
                    break
        elif i == "es":
            for o in xrange(0, 10):
                file = folder + "/" + folder + "_" + i + "_r"+es_runs[options.tc_id] + "_o" + str(o) + "_pop.csv"
                if os.path.exists(file):
                    f = open(file, 'r')
                    line = f.readline()
                    fitness = float(line.split(",")[0])
                    
                    x_i.append((o+1))#*es_gens[options.tc_id])
                    y_i.append(fitness)
                else:
                    break
        elif i == "sa":
            file = folder + "/" + folder + "_" + i + "_r0_iters.csv" 
            if os.path.exists(file):
                f = open(file, 'r')
                lines = f.readlines()
                for l in xrange(0, len(lines), len(lines)/50):
                    iter = float(lines[l].split(",")[0])
                    fitness = float(lines[l].split(",")[1])
                    
                    x_i.append(iter)
                    y_i.append(fitness)
                x_i.append(lines[len(lines)-1].split(",")[0])
                y_i.append(lines[len(lines)-1].split(",")[1])

    
        elif i == "hc":
            file = folder + "/" + folder + "_" + i + "_r0_iters.csv" 
            if os.path.exists(file):
                f = open(file, 'r')
                lines = f.readlines()
                for l in xrange(0, len(lines)):
                    iter = float(lines[l].split(",")[0])
                    fitness = float(lines[l].split(",")[1])
                    
                    x_i.append(iter)
                    y_i.append(fitness)
        y.append(np.array(y_i))
        x.append(np.array(x_i))
        

    fig = plt.figure()
    ax = fig.add_subplot(111)
    plt.xticks(rotation=30)
    ax.plot(x[0],y[0],'k-s', markersize=4)
    #ax.set_xlabel('iteration (x'+str(ga_gens[options.tc_id])+')', fontsize=10)
    ax.set_xlabel('iteration (generation)', fontsize=15)
    ax.set_ylabel('best fitness', fontsize=15)
    plt.setp(ax.get_yticklabels(), visible=False)
    plt.setp(ax.get_xticklabels(), visible=False)
    fig.savefig('/home/ajauhri/quals/paper/FIG/algo_ga.eps', format='eps', dpi=1000)
    plt.clf()
    
    ax = fig.add_subplot(111)
    plt.xticks(rotation=30)
    ax.plot(x[1],y[1],'k-s', markersize=4)
    #ax.set_xlabel('iteration (x'+str(es_gens[options.tc_id])+')', fontsize=15)
    ax.set_xlabel('iteration (generation)', fontsize=15)
    ax.set_ylabel('best fitness', fontsize=15)
    plt.setp(ax.get_yticklabels(), visible=False)
    plt.setp(ax.get_xticklabels(), visible=False)
    #ax.set_xticks([1,2,3,4])
    fig.savefig('/home/ajauhri/quals/paper/FIG/algo_es.eps', format='eps', dpi=1000)
    plt.clf()

    ax = fig.add_subplot(111)
    plt.xticks(rotation=30)
    ax.plot(x[2],y[2],'k-s', markersize=4)
    ax.set_xlabel('iteration', fontsize=15)
    ax.set_ylabel('fitness', fontsize=15)
    plt.setp(ax.get_yticklabels(), visible=False)
    plt.setp(ax.get_xticklabels(), visible=False)
    fig.savefig('/home/ajauhri/quals/paper/FIG/algo_sa.eps', format='eps', dpi=1000)
    plt.clf()

    ax = fig.add_subplot(111)
    plt.xticks(rotation=30)
    ax.plot(x[3],y[3],'k-s', markersize=4)
    ax.set_xlabel('iteration', fontsize=15)
    ax.set_ylabel('fitness', fontsize=15)
    plt.setp(ax.get_yticklabels(), visible=False)
    plt.setp(ax.get_xticklabels(), visible=False)
    fig.savefig('/home/ajauhri/quals/paper/FIG/algo_hc.eps', format='eps', dpi=1000)

    plt.tight_layout()
    #plt.show()

if __name__ == "__main__":
    main()
