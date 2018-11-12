CXX=g++
FLAGS=-std=c++14
all:
	$(CXX) $(FLAGS) -c options.cpp -o options.o
	ar rcs libopto.a options.o
