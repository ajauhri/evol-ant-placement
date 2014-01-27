#! /usr/bin/env python
import sys
import math
import os
from optparse import OptionParser

# argv[1] = test case id 
# argv[2] = algorithm 
# argv[3] = antennas 
# argv[4] = runs 
# argv[5] = generations 
def main():
    parser = OptionParser()
    parser.add_option("-i", "--id", dest="tc_id", help="tc id")
    parser.add_option("-r", "--runs", dest="runs", help="number of runs")
    parser.add_option("-o", "--others", dest="others", help="other identification id")
    
    (options, args) = parser.parse_args()
    if (not options.tc_id) or (not options.runs) or (not options.others):
        parser.error("All options not given")
    test_case_name = "tc" + options.tc_id

    # GA
    fit_agg = 0
    for r in xrange(0, int(options.runs)):
        best = 1.0
        for o in xrange(0, int(options.others)):
            file = test_case_name + "/" + test_case_name + "_ga" + "_r" + str(r) + "_o" + str(o) + "_pop.csv"
            if os.path.exists(file):
                fd = open(file, 'r')
                first_line = fd.readline()
                fitness = float(first_line.split(",")[0])
                if fitness < best:
                    best = fitness
            else:
                #print "breaked on %d" % o
                break
        fit_agg += best
    ex_result_fit = float(open(test_case_name+"/"+test_case_name+"_ex_r0_o0_pop.csv").readline().split(",")[0])
    fit_agg /= int(options.runs)
    print "ga agg=%f, ex_best=%f" % (fit_agg, ex_result_fit)
    result = math.fabs(ex_result_fit - fit_agg)
    # print "ga result for " + test_case_name + " " + str(result)

    #ES 
    fit_agg = 0
    for r in xrange(0, int(options.runs)):
        best = 1.0
        for o in xrange(0, int(options.others)):
            file = test_case_name + "/" + test_case_name + "_es" + "_r" + str(r) + "_o" + str(o) + "_pop.csv"
            if os.path.exists(file):
                fd = open(file, 'r')
                first_line = fd.readline()
                fitness = float(first_line.split(",")[0])
                if fitness < best:
                    best = fitness
            else:
                #print "breaked on %d" % o
                break
        fit_agg += best
    ex_result_fit = float(open(test_case_name+"/"+test_case_name+"_ex_r0_o0_pop.csv").readline().split(",")[0])
    fit_agg /= int(options.runs)
    print "es agg=%f, ex_best=%f" % (fit_agg, ex_result_fit)

    #SA
    fit_agg = 0
    for r in xrange(0, int(options.runs)):
        best = 1.0
        file = test_case_name + "/" + test_case_name + "_sa" + "_r" + str(r) + ".iters.csv"
        lines = open(file).readlines()
        for line in lines:
            fitness = float(line.split(",")[1])
            if fitness < best:
                best = fitness
        fit_agg += best
    ex_result_fit = float(open(test_case_name+"/"+test_case_name+"_ex_r0_o0_pop.csv").readline().split(",")[0])
    fit_agg /= int(options.runs)
    print "sa agg=%f, ex_best=%f" % (fit_agg, ex_result_fit)

    #HC
    fit_agg = 0
    for r in xrange(0, int(options.runs)):
        best = 1.0
        file = test_case_name + "/" + test_case_name + "_hc" + "_r" + str(r) + "iters.csv"
        lines = open(file).readlines()
        for line in lines:
            fitness = float(line.split(",")[1])
            if fitness < best:
                best = fitness
        fit_agg += best
    ex_result_fit = float(open(test_case_name+"/"+test_case_name+"_ex_r0_o0_pop.csv").readline().split(",")[0])
    fit_agg /= int(options.runs)
    print "hc agg=%f, ex_best=%f" % (fit_agg, ex_result_fit)
    
    

    

if __name__ == "__main__":
    main()
