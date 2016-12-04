FILES = para_b.cpp
all:
	g++ -fopenmp -Wall -g ${FILES} -o test
