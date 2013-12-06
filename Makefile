PROJ = evol_ant
CC = g++

CFLAGS = -c -Wall -std=c++11 -I.
LDFLAGS = -g -L/usr/lib/x86_64-linux-gnu
LIBS = -lboost_program_options -lboost_system -lboost_filesystem -llua5.2
OBJS = $(patsubst %.cpp,obj/%.o,$(wildcard *.cpp))


all : $(PROJ)

$(PROJ) : $(OBJS)
	$(CC) $(LDFLAGS) $^ -o $@ $(LIBS)

obj/%.o: %.cpp 
	$(CC) $(CFLAGS) $< -o $@

clean: 
	rm -f $(PROJ) $(OBJS) 


