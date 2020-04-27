CXX = g++
CXXFLAGS = -Wall -g -O3

all: partition

partition: partition.o
	$(CXX) $(CXXFLAGS) -o partition partition.o

partition.o: partition.cpp
	$(CXX) $(CXXFLAGS) -c partition.cpp

clean:
	rm partition *.o