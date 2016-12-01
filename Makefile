FILES = para_a.cpp
all:
	g++ -fopenmp -Wall -g ${FILES} -o test
