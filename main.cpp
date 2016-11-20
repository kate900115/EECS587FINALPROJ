#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <map>

#define size 500
#define MAX 10000000
#define MIN -10000000

using namespace std;

class body
{
	public:
		double mass_center;
		double mass_sum;
		double array_num;
		
		//constructor
		//mass initialized as -100 indicate the result of the center of mass is not computed
		//array_num initialized as -1 indicate it don't contain any body
		body(){ mass_center=-100; mass_sum = -100; array_num = -1;}
};



int main(int argc, char** argv)
{
	double max_x=MIN;
	double min_x=MAX;
	double max_y=MIN;
	double min_y=MAX;
	string FileName = string(argv[1]);
	ifstream InFile.open(FileName.c_str());
	
	
	double x[size];
	double y[size];
	double m[size];
	double vx[size];
	double vy[size];
	
	map<int, body> node;
	
	string line;
	int index = 0;

	
	
	//read the input file
	//find the max_x, max_y, min_x, min_y
	while (getline(InFile, line))
	{
		stringstream sstr;
		string word;
		sstr << line;
		sstr >> word;
		x[index] = atof(word.c_str());
		max_x = x[index]>MAX? x[index]:MAX;
		min_x = x[index]<MIN? x[index]:MIN;
		sstr >> word;
		y[index] = atof(word.c_str());
		max_y = y[index]>MAX? y[index]:MAX;
		min_y = y[index]<MIN? y[index]:MIN;
		sstr >> word;
		m[index] = atof(word.c_str());
		sstr >> word;
		vx[index] = atof(word.c_str());
		sstr >> word;
		vy[index] = atof(word.c_str());
		index++;
	}	

	//insert each body to the tree structure
	node[0] = 
	for (int i=1; i<size; i++)
	{
		
	}


	//update the mass_sum of from the leaf to the top
	return 0;

}
