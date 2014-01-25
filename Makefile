PROJ = evol_ant
CC = g++

CFLAGS = -c -Wall -std=c++11 -I. -I/afs/andrew.cmu.edu/usr9/ajauhri/boost/include -I/afs/andrew.cmu.edu/usr9/ajauhri/lua/include
LDFLAGS = -g -L/usr/lib -L/afs/andrew.cmu.edu/usr9/ajauhri/lua/lib -L/afs/andrew.cmu.edu/usr9/ajauhri/boost/lib
LIBS = -lboost_program_options -lboost_system -lboost_filesystem -llua -ldl
OBJS = $(patsubst %.cpp,obj/%.o,$(wildcard *.cpp))

all : $(PROJ)

$(PROJ) : $(OBJS)
	$(CC) $(LDFLAGS) $^ -o $@ $(LIBS)

obj/%.o: %.cpp 
	$(CC) $(CFLAGS) $< -o $@

clean: 
	rm -f $(PROJ) $(OBJS)


