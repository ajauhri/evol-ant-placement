#! /usr/bin/env python
# quick hack to generate all test cases such each antenna is present on all points of the platform
# sys.argv[1] - should be platform
# sys.argv[2:] - should be all antenna(s)
import sys

def main():
    try:
        out = open('tc', 'w')
        with open(sys.argv[1]) as p:
            p_lines = p.readlines()
            p_lines = map(lambda x: x.split(" "), p_lines) 
            out.write("load_platform(\"" + sys.argv[1] + "\")\n")
            for ant in xrange(0, len(sys.argv)-2):
                with open(sys.argv[ant + 2]) as ant_in:
                    out.write("add_antenna(\"" + sys.argv[ant + 2] + "\")\n")
                    for line in p_lines:
                        if line[0] in "GW":
                            points = zip(*[iter(line[3:-1])]*3)
                            for point in points:
                                out.write("add_point(" + point[0] + "," + point[1] + "," + point[2] + ")\n")
                    ant_in.close()
            p.close()
            out.close()

    except EnvironmentError, e:
        print e

if __name__ == "__main__":
    main()
