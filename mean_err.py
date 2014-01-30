#! /usr/bin/env python
from __future__ import division
import sys
import math
import os
from optparse import OptionParser

ga_gens = {'1':500, '2':3600, '3':8500, '4':1500 }
es_gens = {'1':480, '2':3850, '3':8400, '4':1540 }

def main():
    parser = OptionParser()
    parser.add_option("-i", "--id", dest="tc_id", help="tc id")
    parser.add_option("-r", "--runs", dest="runs", help="number of runs")
    parser.add_option("-o", "--others", dest="others", help="other identification id")
    
    (options, args) = parser.parse_args()
    if (not options.tc_id) or (not options.runs) or (not options.others):
        parser.error("All options not given")
    test_case_name = "tc" + options.tc_id
    ex_result_fit = float(open(test_case_name+"/"+test_case_name+"_ex_r0_o0_pop.csv").readline().split(",")[0])
    # GA
    fit_agg = 0
    gen_agg = 0
    count = 0
    for r in xrange(0, int(options.runs)):
        best = 1.0
        gen = 0
        for o in xrange(0, int(options.others)):
            file = test_case_name + "/" + test_case_name + "_ga" + "_r" + str(r) + "_o" + str(o) + "_pop.csv"
            if os.path.exists(file):
                count += 1
                fd = open(file, 'r')
                first_line = fd.readline()
                fitness = float(first_line.split(",")[0])
                if fitness < best and fitness != 0.49864:
                    best = fitness
                    gen = o
                    print best
            else:
                print "breaked on %d" % o
                break
        fit_agg += best
        print '**'
        gen_agg += (gen+1)*ga_gens[options.tc_id]
    fit_agg /= int(options.runs)
    gen_agg /= int(options.runs)
    result = math.fabs(ex_result_fit - fit_agg)
    print "files found = %d" % count
    print "ga mean best fitness=%f, ex_best=%f" % (fit_agg, ex_result_fit)
    print "ga diff mean - best= %f" %(result)
    print "ga mean runs=%f" % (gen_agg)
    print "*********"

    #ES 
    fit_agg = 0
    gen_agg = 0
    count = 0
    for r in xrange(0, int(options.runs)):
        best = 1.0
        for o in xrange(0, int(options.others)):
            file = test_case_name + "/" + test_case_name + "_es" + "_r" + str(r) + "_o" + str(o) + "_pop.csv"
            if os.path.exists(file):
                count += 1
                fd = open(file, 'r')
                first_line = fd.readline()
                fitness = float(first_line.split(",")[0])
                if fitness < best:
                    best = fitness
                    gen = o
                    print best
            else:
                break
        fit_agg += best
        print '**'
        gen_agg += (gen+1)*es_gens[options.tc_id]

    fit_agg /= int(options.runs)
    gen_agg /= int(options.runs)
    result = math.fabs(ex_result_fit - fit_agg)
    print "files found = %d" % count
    print "es mean best fitness=%f, ex_best=%f" % (fit_agg, ex_result_fit)
    print "es diff mean - best= %f" %(result)
    print "es mean runs=%f" % (gen_agg)
    print "*********"

    #SA
    fit_agg = 0
    iter_agg = 0
    count = 0
    for r in xrange(0, int(options.runs)):
        best = 1
        iter = 0
        file = test_case_name + "/" + test_case_name + "_sa" + "_r" + str(r) + "_iters.csv"
        try:
            lines = open(file).readlines()
            count += 1
            for line in lines:
                fitness = float(line.split(",")[1])
                if fitness < best:
                    best = fitness
                    iter = int(line.split(",")[0])

            fit_agg += best
            if best >= 1:
                print '***WRONG', file
        except IOError:
                print '***DEAD', file
        iter_agg += iter

    fit_agg /= int(options.runs)
    iter_agg /= int(options.runs)
    result = math.fabs(ex_result_fit - fit_agg)
    print "files found = %d" % count
    print "sa mean best fitness=%f, ex_best=%f" % (fit_agg,  ex_result_fit)
    print "sa result for %f" %(result)
    print "sa mean runs=%f" % (iter_agg)
    print "*********"

    #HC
    fit_agg = 0
    iter_agg = 0
    count = 0
    for r in xrange(0, int(options.runs)):
        best = 1.0
        iter = 0
        file = test_case_name + "/" + test_case_name + "_hc" + "_r" + str(r) + "iters.csv"
        try:
            lines = open(file).readlines()
            count += 1
            for line in lines:
                fitness = float(line.split(",")[1])
                if fitness < best:
                    best = fitness
                    iter = int(line.split(",")[0])

            fit_agg += best
            if best >= 1:
                print '***WRONG', file
        except IOError:
            print '***DEAD', file
        iter_agg += iter

    fit_agg /= int(options.runs)
    iter_agg /= int(options.runs)
    result = math.fabs(ex_result_fit - fit_agg)
    print "files found = %d" % count
    print "hc mean best fitness=%f,  ex_best=%f" % (fit_agg, ex_result_fit)
    print "hc result for %f" %(result)
    print "hc mean runs=%f" % (iter_agg)
    print "*********"
    
if __name__ == "__main__":
    main()
