PROJ = evol_ant
CC = g++

CFLAGS = -c -Wall -I./rapidxml -std=c++11
LDFLAGS = -g -L/usr/lib/x86_64-linux-gnu
LIBS = -lboost_program_options -lboost_system -lboost_filesystem
OBJS = $(patsubst %.cpp,obj/%.o,$(wildcard *.cpp))


all : $(PROJ)

$(PROJ) : $(OBJS)
	$(CC) $(LDFLAGS) $^ -o $@ $(LIBS)

obj/%.o: %.cpp
	$(CC) $(CFLAGS) $< -o $@

clean: 
	rm -f $(PROJ) $(OBJS) 


