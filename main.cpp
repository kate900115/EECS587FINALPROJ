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

double max_x;
double min_x;
double max_y;
double min_y;

class body
{
	public:
		double mass_center;
		double mass_sum;
		double array_num;
		double NW_x;
		double NW_y;
		double NE_x;
		double NE_y;
		double SW_x;
		double SW_y;
		double SE_x;
		double SE_y;
		
		//constructor
		//mass initialized as -100 indicate the result of the center of mass is not computed
		//array_num initialized as -1 indicate it don't contain any body
		body()
		{
			mass_center=-100; 
			mass_sum = -100; 
			array_num = -1;
			NW_x = min_x;
			NW_y = max_y;
			NE_x = max_x;
			NE_y = max_y; 
			SW_x = min_x;
			SW_y = min_y;
			SE_x = max_x;
			SE_y = min_y;
		}
};



int main(int argc, char** argv)
{
	// the max coordinate and the minimum coordinate of 
	// the 2D-space
	double max_x=MIN;
	double min_x=MAX;
	double max_y=MIN;
	double min_y=MAX;
	string FileName = string(argv[1]);
	ifstream InFile.open(FileName.c_str());
	
	// coordinate
	double x[size];
	double y[size];
	// mass
	double m[size];
	
	// speed
	double vx[size];
	double vy[size];
	
	map<int, body> nodes;
	map<int, body>::iterator it;
	
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
		max_x = x[index]>max_x? x[index]:max_x;
		min_x = x[index]<min_x? x[index]:min_x;
		sstr >> word;
		y[index] = atof(word.c_str());
		max_y = y[index]>max_y? y[index]:max_y;
		min_y = y[index]<min_y? y[index]:min_y;
		sstr >> word;
		m[index] = atof(word.c_str());
		sstr >> word;
		vx[index] = atof(word.c_str());
		sstr >> word;
		vy[index] = atof(word.c_str());
		index++;
	}

	InFile.close();	

	//insert each body to the tree structure
	body first;
	first.array_num = 0;
	nodes[0] = first;

	for (int i=1; i<size; i++)
	{
		int x_start = min_x;
		int x_end = max_x;
		int y_start = min_y;
		int y_end = max_y;
		int j=0;
		while (nodes[j].array_num!=-1)
		{
			if ((x[i]<(x_start+x_end)/2)&&(y[i]<(y_start+y_end)/2))
			{
				j=4*j+1;
				// if the nodes doesn't exist
				// insert the new node.
				if (nodes.find(j)!=NULL)
				{
					// create the first node
					body NewNode; 
					NewNode.array_num = i;
					map[j]=NewNode;
					// create the rest 3 nodes
					body NewNode1;
					map[j+1] = NewNode1;
					body NewNode2;
					map[j+2] = NewNode2;
					body NewNode3;
					map[j+3] = NewNode3;
					break;

				}
				// if the node exist
				// but have not map to a node
				else if (nodes[j].array_num==-1)
				{
					nodes[j].array_num = i;
					break;
				}
				// update the x_start, x_end, y_start, y_end;
				x_end=(x_start+x_end)/2;
				y_end=(y_start+y_end)/2;
			}
			else if ((x[i]<(x_start+x_end)/2)&&(y[i]>=(y_start+y_end)/2))
			{
				j=4*j+2;
				// if the nodes doesn't exist
				// insert the new node.
				if (nodes.find(j)!=NULL)
				{
					// create the first node
					body NewNode; 
					NewNode.array_num = i;
					map[j]=NewNode;
					// create the rest 3 nodes
					body NewNode1;
					map[j-1] = NewNode1;
					body NewNode2;
					map[j+1] = NewNode2;
					body NewNode3;
					map[j+2] = NewNode3;
					break;

				}
				// if the node exist
				// but have not map to a node
				else if (nodes[j].array_num==-1)
				{
					nodes[j].array_num = i;
					break;
				}
				// update the x_start, x_end, y_start, y_end;
				x_end=(x_start+x_end)/2;
				y_start=(y_start+y_end)/2;
			}
			else if ((x[i]>=(start_x+end_x)/2)&&(y[i]<(start_y+end_y)/2))
			{
				j=4*j+3;
				// if the nodes doesn't exist
				// insert the new node.
				if (nodes.find(j)!=NULL)
				{
					// create the first node
					body NewNode; 
					NewNode.array_num = i;
					map[j]=NewNode;
					// create the rest 3 nodes
					body NewNode1;
					map[j-2] = NewNode1;
					body NewNode2;
					map[j-1] = NewNode2;
					body NewNode3;
					map[j+1] = NewNode3;
					break;

				}
				// if the node exist
				// but have not map to a node
				else if (nodes[j].array_num==-1)
				{
					nodes[j].array_num = i;
					break;
				}
				// update the x_start, x_end, y_start, y_end;
				x_start=(x_start+x_end)/2;
				y_end=(y_start+y_end)/2;
			}
			else
			{
				j=4*j+4;
				// if the nodes doesn't exist
				// insert the new node.
				if (nodes.find(j)!=NULL)
				{
					// create the first node
					body NewNode; 
					NewNode.array_num = i;
					map[j]=NewNode;
					// create the rest 3 nodes
					body NewNode1;
					map[j-2] = NewNode1;
					body NewNode2;
					map[j-1] = NewNode2;
					body NewNode3;
					map[j+1] = NewNode3;
					break;

				}
				// if the node exist
				// but have not map to a node
				else if (nodes[j].array_num==-1)
				{
					nodes[j].array_num = i;
					break;
				}
				// update the x_start, x_end, y_start, y_end;
				x_start=(x_start+x_end)/2;
				y_start=(y_start+y_end)/2;
			}
		}
	}


	//update the mass_sum of from the leaf to the top
	return 0;

}
