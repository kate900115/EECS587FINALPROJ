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
	ifstream InFile;
	InFile.open(FileName.c_str());
	
	// coordinate
	double x[size];
	double y[size];
	// mass
	double m[size];
	
	// speed
	double vx[size];
	double vy[size];

	// 
	body quadtree[size*4];
	
	map<int, body> nodes;
	map<int, body>::iterator it;
	map<int, body>::reverse_iterator rit;
	
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
	cout<<"max_x="<<max_x<<", max_y="<<max_y<<endl;
	cout<<"min_x="<<min_x<<", min_y="<<min_y<<endl;

	//insert each body to the tree structure
	quadtree[0].array_num = 0;
	quadtree[0].mass_center = m[0];
	quadtree[0].mass_sum = m[0];
	quadtree[0].NW_x = min_x;
	quadtree[0].NW_y = min_y;
	quadtree[0].SE_x = max_x;
	quadtree[0].SE_y = max_y;

	for (int i=1; i<index; i++)
	{
		int x_start = min_x;
		int x_end = max_x;
		int y_start = min_y;
		int y_end = max_y;
		int j=0;
		while (quadtree[j].array_num!=-1)
		{
			if ((x[i]<(x_start+x_end)/2)&&(y[i]<(y_start+y_end)/2))
			{
				j=4*j+1;
				x_end=(x_start+x_end)/2;
				y_end=(y_start+y_end)/2;
				// if the nodes doesn't exist
				// insert the new node.
				if (quadtree[j].array_num==-1)
				{
					// create the first node
					quadtree[j].array_num = i;
					quadtree[j].mass_center = m[i];
					quadtree[j].mass_sum = m[i];
					quadtree[j].NW_x = x_start;
					quadtree[j].NW_y = y_start;
					quadtree[j].SE_x = x_end;
					quadtree[j].SE_y = y_end;
					// create the rest 3 nodes
					quadtree[j+1].array_num = -2;
					quadtree[j+1].NW_x = x_start;
					quadtree[j+1].NW_y = y_start;
					quadtree[j+1].SE_x = x_end;
					quadtree[j+1].SE_y = y_end;

					quadtree[j+2].array_num = -2;
					quadtree[j+2].NW_x = x_start;
					quadtree[j+2].NW_y = y_start;
					quadtree[j+2].SE_x = x_end;
					quadtree[j+2].SE_y = y_end;

					quadtree[j+3].array_num = -2;
					quadtree[j+3].NW_x = x_start;
					quadtree[j+3].NW_y = y_start;
					quadtree[j+3].SE_x = x_end;
					quadtree[j+3].SE_y = y_end;
					break;
				}
				// if the node exist
				// but have not map to a node
				else if (quadtree[j].array_num==-2)
				{
					quadtree[j].array_num = i;
					quadtree[j].mass_center = m[i];
					quadtree[j].mass_sum = m[i];
					quadtree[j].NW_x = x_start;
					quadtree[j].NW_y = y_start;
					quadtree[j].SE_x = x_end;
					quadtree[j].SE_y = y_end;
					break;
				}
				// if the node exist 
				// and have been already mapped to a node
				// move this node to next level
				else
				{	
					// if the node is the leaf
					if (quadtree[4*j+1].array_num==-1)
					{
						int temp = quadtree[j].array_num;
						quadtree[j].array_num = -1;
						
						if ((x[temp]<(x_start+x_end)/2)&&(y[temp]<(y_start+y_end)/2))
						{
							// create the first node
							// put the original node here
							quadtree[4*j+1].array_num = temp;
							quadtree[4*j+1].mass_center = m[temp];
							quadtree[4*j+1].mass_sum = m[temp];
							quadtree[4*j+1].NW_x = x_start;
							quadtree[4*j+1].NW_y = y_start;
							quadtree[4*j+1].SE_x = (x_start+x_end)/2;
							quadtree[4*j+1].SE_y = (y_start+y_end)/2;
							// create the second node
							quadtree[4*j+2].array_num = -2;
							quadtree[4*j+2].NW_x = x_start;
							quadtree[4*j+2].NW_y = y_start;
							quadtree[4*j+2].SE_x = (x_start+x_end)/2;
							quadtree[4*j+2].SE_y = (y_start+y_end)/2;
							// create the third node
							quadtree[4*j+3].array_num = -2;
							quadtree[4*j+3].NW_x = x_start;
							quadtree[4*j+3].NW_y = y_start;
							quadtree[4*j+3].SE_x = (x_start+x_end)/2;
							quadtree[4*j+3].SE_y = (y_start+y_end)/2;
							// create the fourth node
							quadtree[4*j+4].array_num = -2;
							quadtree[4*j+4].NW_x = x_start;
							quadtree[4*j+4].NW_y = y_start;
							quadtree[4*j+4].SE_x = (x_start+x_end)/2;
							quadtree[4*j+4].SE_y = (y_start+y_end)/2;
						}
						else if ((x[temp]<(x_start+x_end)/2)&&(y[temp]>=(y_start+y_end)/2))
						{
							// create the first node
							// put the original node here
							quadtree[4*j+1].array_num = -2;
							quadtree[4*j+1].NW_x = x_start;
							quadtree[4*j+1].NW_y = (y_start+y_end)/2;
							quadtree[4*j+1].SE_x = (x_start+x_end)/2;
							quadtree[4*j+1].SE_y = y_end;
							// create the second node
							quadtree[4*j+2].array_num = temp;
							quadtree[4*j+2].mass_center = m[temp];
							quadtree[4*j+2].mass_sum = m[temp];
							quadtree[4*j+2].NW_x = x_start;
							quadtree[4*j+2].NW_y = (y_start+y_end)/2;
							quadtree[4*j+2].SE_x = (x_start+x_end)/2;
							quadtree[4*j+2].SE_y = y_end;
							// create the third node
							quadtree[4*j+3].array_num = -2;
							quadtree[4*j+3].NW_x = x_start;
							quadtree[4*j+3].NW_y = (y_start+y_end)/2;
							quadtree[4*j+3].SE_x = (x_start+x_end)/2;
							quadtree[4*j+3].SE_y = y_end;
							// create the fourth node
							quadtree[4*j+4].array_num = -2;
							quadtree[4*j+4].NW_x = x_start;
							quadtree[4*j+4].NW_y = (y_start+y_end)/2;
							quadtree[4*j+4].SE_x = (x_start+x_end)/2;
							quadtree[4*j+4].SE_y = y_end;
						}
						else if ((x[temp]>=(x_start+x_end)/2)&&(y[temp]<(y_start+y_end)/2))
						{
							// create the first node
							// put the original node here
							quadtree[4*j+1].array_num = -2;
							quadtree[4*j+1].NW_x = (x_start+x_end)/2;
							quadtree[4*j+1].NW_y = y_start;
							quadtree[4*j+1].SE_x = x_end;
							quadtree[4*j+1].SE_y = (y_start+y_end)/2;
							// create the second node
							quadtree[4*j+2].array_num = -2;
							quadtree[4*j+2].NW_x = (x_start+x_end)/2;
							quadtree[4*j+2].NW_y = y_start;
							quadtree[4*j+2].SE_x = x_end;
							quadtree[4*j+2].SE_y = (y_start+y_end)/2;
							// create the third node
							quadtree[4*j+3].array_num = temp;
							quadtree[4*j+3].mass_center = m[temp];
							quadtree[4*j+3].mass_sum = m[temp];
							quadtree[4*j+3].NW_x = (x_start+x_end)/2;
							quadtree[4*j+3].NW_y = y_start;
							quadtree[4*j+3].SE_x = x_end;
							quadtree[4*j+3].SE_y = (y_start+y_end)/2;
							// create the fourth node
							quadtree[4*j+4].array_num = -2;
							quadtree[4*j+4].NW_x = (x_start+x_end)/2;
							quadtree[4*j+4].NW_y = y_start;
							quadtree[4*j+4].SE_x = x_end;
							quadtree[4*j+4].SE_y = (y_start+y_end)/2;
						}
						else
						{
							// create the first node
							// put the original node here
							quadtree[4*j+1].array_num = -2;
							quadtree[4*j+1].NW_x = (x_start+x_end)/2;
							quadtree[4*j+1].NW_y = (y_start+y_end)/2;
							quadtree[4*j+1].SE_x = x_end;
							quadtree[4*j+1].SE_y = y_end;
							// create the second node
							quadtree[4*j+2].array_num = -2;
							quadtree[4*j+2].NW_x = (x_start+x_end)/2;
							quadtree[4*j+2].NW_y = (y_start+y_end)/2;
							quadtree[4*j+2].SE_x = x_end;
							quadtree[4*j+2].SE_y = y_end;
							// create the third node
							quadtree[4*j+3].array_num = -2;
							quadtree[4*j+3].NW_x = (x_start+x_end)/2;
							quadtree[4*j+3].NW_y = (y_start+y_end)/2;
							quadtree[4*j+3].SE_x = x_end;
							quadtree[4*j+3].SE_y = y_end;
							// create the fourth node
							quadtree[4*j+4].array_num = temp;
							quadtree[4*j+4].mass_center = m[temp];
							quadtree[4*j+4].mass_sum = m[temp];
							quadtree[4*j+4].NW_x = (x_start+x_end)/2;
							quadtree[4*j+4].NW_y = (y_start+y_end)/2;
							quadtree[4*j+4].SE_x = x_end;
							quadtree[4*j+4].SE_y = y_end;
						}
					}
				}
			}
			else if ((x[i]<(x_start+x_end)/2)&&(y[i]>=(y_start+y_end)/2))
			{
				j=4*j+2;
				x_end=(x_start+x_end)/2;
				y_start=(y_start+y_end)/2;
				// if the nodes doesn't exist
				// insert the new node.
				if (quadtree[j].array_num==-1)
				{
					// create the first node
					quadtree[j].array_num = i;
					quadtree[j].mass_center = m[i];
					quadtree[j].mass_sum = m[i];
					quadtree[j].NW_x = x_start;
					quadtree[j].NW_y = y_start;
					quadtree[j].SE_x = x_end;
					quadtree[j].SE_y = y_end;
					// create the rest 3 nodes
					quadtree[j-1].array_num = -2;
					quadtree[j-1].NW_x = x_start;
					quadtree[j-1].NW_y = y_start;
					quadtree[j-1].SE_x = x_end;
					quadtree[j-1].SE_y = y_end;

					quadtree[j+1].array_num = -2;
					quadtree[j+1].NW_x = x_start;
					quadtree[j+1].NW_y = y_start;
					quadtree[j+1].SE_x = x_end;
					quadtree[j+1].SE_y = y_end;

					quadtree[j+2].array_num = -2;
					quadtree[j+2].NW_x = x_start;
					quadtree[j+2].NW_y = y_start;
					quadtree[j+2].SE_x = x_end;
					quadtree[j+2].SE_y = y_end;
					break;
				}
				// if the node exist
				// but have not map to a node
				else if (quadtree[j].array_num==-2)
				{
					quadtree[j].array_num = i;
					quadtree[j].mass_center = m[i];
					quadtree[j].mass_sum = m[i];
					quadtree[j].NW_x = x_start;
					quadtree[j].NW_y = y_start;
					quadtree[j].SE_x = x_end;
					quadtree[j].SE_y = y_end;
					break;
				}
				// if the node exist 
				// and have been already mapped to a node
				// move this node to next level
				else
				{	
					// if the node is the leaf
					if (quadtree[4*j+1].array_num==-1)
					{
						int temp = quadtree[j].array_num;
						quadtree[j].array_num = -1;
						
						if ((x[temp]<(x_start+x_end)/2)&&(y[temp]<(y_start+y_end)/2))
						{
							// create the first node
							// put the original node here
							quadtree[4*j+1].array_num = temp;
							quadtree[4*j+1].mass_center = m[temp];
							quadtree[4*j+1].mass_sum = m[temp];
							quadtree[4*j+1].NW_x = x_start;
							quadtree[4*j+1].NW_y = y_start;
							quadtree[4*j+1].SE_x = (x_start+x_end)/2;
							quadtree[4*j+1].SE_y = (y_start+y_end)/2;
							// create the second node
							quadtree[4*j+2].array_num = -2;
							quadtree[4*j+2].NW_x = x_start;
							quadtree[4*j+2].NW_y = y_start;
							quadtree[4*j+2].SE_x = (x_start+x_end)/2;
							quadtree[4*j+2].SE_y = (y_start+y_end)/2;
							// create the third node
							quadtree[4*j+3].array_num = -2;
							quadtree[4*j+3].NW_x = x_start;
							quadtree[4*j+3].NW_y = y_start;
							quadtree[4*j+3].SE_x = (x_start+x_end)/2;
							quadtree[4*j+3].SE_y = (y_start+y_end)/2;
							// create the fourth node
							quadtree[4*j+4].array_num = -2;
							quadtree[4*j+4].NW_x = x_start;
							quadtree[4*j+4].NW_y = y_start;
							quadtree[4*j+4].SE_x = (x_start+x_end)/2;
							quadtree[4*j+4].SE_y = (y_start+y_end)/2;
						}
						else if ((x[temp]<(x_start+x_end)/2)&&(y[temp]>=(y_start+y_end)/2))
						{
							// create the first node
							// put the original node here
							quadtree[4*j+1].array_num = -2;
							quadtree[4*j+1].NW_x = x_start;
							quadtree[4*j+1].NW_y = (y_start+y_end)/2;
							quadtree[4*j+1].SE_x = (x_start+x_end)/2;
							quadtree[4*j+1].SE_y = y_end;
							// create the second node
							quadtree[4*j+2].array_num = temp;
							quadtree[4*j+2].mass_center = m[temp];
							quadtree[4*j+2].mass_sum = m[temp];
							quadtree[4*j+2].NW_x = x_start;
							quadtree[4*j+2].NW_y = (y_start+y_end)/2;
							quadtree[4*j+2].SE_x = (x_start+x_end)/2;
							quadtree[4*j+2].SE_y = y_end;
							// create the third node
							quadtree[4*j+3].array_num = -2;
							quadtree[4*j+3].NW_x = x_start;
							quadtree[4*j+3].NW_y = (y_start+y_end)/2;
							quadtree[4*j+3].SE_x = (x_start+x_end)/2;
							quadtree[4*j+3].SE_y = y_end;
							// create the fourth node
							quadtree[4*j+4].array_num = -2;
							quadtree[4*j+4].NW_x = x_start;
							quadtree[4*j+4].NW_y = (y_start+y_end)/2;
							quadtree[4*j+4].SE_x = (x_start+x_end)/2;
							quadtree[4*j+4].SE_y = y_end;
						}
						else if ((x[temp]>=(x_start+x_end)/2)&&(y[temp]<(y_start+y_end)/2))
						{
							// create the first node
							// put the original node here
							quadtree[4*j+1].array_num = -2;
							quadtree[4*j+1].NW_x = (x_start+x_end)/2;
							quadtree[4*j+1].NW_y = y_start;
							quadtree[4*j+1].SE_x = x_end;
							quadtree[4*j+1].SE_y = (y_start+y_end)/2;
							// create the second node
							quadtree[4*j+2].array_num = -2;
							quadtree[4*j+2].NW_x = (x_start+x_end)/2;
							quadtree[4*j+2].NW_y = y_start;
							quadtree[4*j+2].SE_x = x_end;
							quadtree[4*j+2].SE_y = (y_start+y_end)/2;
							// create the third node
							quadtree[4*j+3].array_num = temp;
							quadtree[4*j+3].mass_center = m[temp];
							quadtree[4*j+3].mass_sum = m[temp];
							quadtree[4*j+3].NW_x = (x_start+x_end)/2;
							quadtree[4*j+3].NW_y = y_start;
							quadtree[4*j+3].SE_x = x_end;
							quadtree[4*j+3].SE_y = (y_start+y_end)/2;
							// create the fourth node
							quadtree[4*j+4].array_num = -2;
							quadtree[4*j+4].NW_x = (x_start+x_end)/2;
							quadtree[4*j+4].NW_y = y_start;
							quadtree[4*j+4].SE_x = x_end;
							quadtree[4*j+4].SE_y = (y_start+y_end)/2;
						}
						else
						{
							// create the first node
							// put the original node here
							quadtree[4*j+1].array_num = -2;
							quadtree[4*j+1].NW_x = (x_start+x_end)/2;
							quadtree[4*j+1].NW_y = (y_start+y_end)/2;
							quadtree[4*j+1].SE_x = x_end;
							quadtree[4*j+1].SE_y = y_end;
							// create the second node
							quadtree[4*j+2].array_num = -2;
							quadtree[4*j+2].NW_x = (x_start+x_end)/2;
							quadtree[4*j+2].NW_y = (y_start+y_end)/2;
							quadtree[4*j+2].SE_x = x_end;
							quadtree[4*j+2].SE_y = y_end;
							// create the third node
							quadtree[4*j+3].array_num = -2;
							quadtree[4*j+3].NW_x = (x_start+x_end)/2;
							quadtree[4*j+3].NW_y = (y_start+y_end)/2;
							quadtree[4*j+3].SE_x = x_end;
							quadtree[4*j+3].SE_y = y_end;
							// create the fourth node
							quadtree[4*j+4].array_num = temp;
							quadtree[4*j+4].mass_center = m[temp];
							quadtree[4*j+4].mass_sum = m[temp];
							quadtree[4*j+4].NW_x = (x_start+x_end)/2;
							quadtree[4*j+4].NW_y = (y_start+y_end)/2;
							quadtree[4*j+4].SE_x = x_end;
							quadtree[4*j+4].SE_y = y_end;
						}
					}
				}
			}
			else if ((x[i]>=(x_start+x_end)/2)&&(y[i]<(y_start+y_end)/2))
			{
				j=4*j+3;
				x_start=(x_start+x_end)/2;
				y_end=(y_start+y_end)/2;
				// if the nodes doesn't exist
				// insert the new node.
				if (quadtree[j].array_num==-1)
				{
					// create the first node
					quadtree[j].array_num = i;
					quadtree[j].mass_center = m[i];
					quadtree[j].mass_sum = m[i];
					quadtree[j].NW_x = x_start;
					quadtree[j].NW_y = y_start;
					quadtree[j].SE_x = x_end;
					quadtree[j].SE_y = y_end;
					// create the rest 3 nodes
					quadtree[j-2].array_num = -2;
					quadtree[j-2].NW_x = x_start;
					quadtree[j-2].NW_y = y_start;
					quadtree[j-2].SE_x = x_end;
					quadtree[j-2].SE_y = y_end;

					quadtree[j-1].array_num = -2;
					quadtree[j-1].NW_x = x_start;
					quadtree[j-1].NW_y = y_start;
					quadtree[j-1].SE_x = x_end;
					quadtree[j-1].SE_y = y_end;

					quadtree[j+1].array_num = -2;
					quadtree[j+1].NW_x = x_start;
					quadtree[j+1].NW_y = y_start;
					quadtree[j+1].SE_x = x_end;
					quadtree[j+1].SE_y = y_end;
					break;
				}
				// if the node exist
				// but have not map to a node
				else if (quadtree[j].array_num==-2)
				{
					quadtree[j].array_num = i;
					quadtree[j].mass_center = m[i];
					quadtree[j].mass_sum = m[i];
					quadtree[j].NW_x = x_start;
					quadtree[j].NW_y = y_start;
					quadtree[j].SE_x = x_end;
					quadtree[j].SE_y = y_end;
					break;
				}
				// if the node exist 
				// and have been already mapped to a node
				// move this node to next level
				else
				{	
					// if the node is the leaf
					if (quadtree[4*j+1].array_num==-1)
					{
						int temp = quadtree[j].array_num;
						quadtree[j].array_num = -1;
						
						if ((x[temp]<(x_start+x_end)/2)&&(y[temp]<(y_start+y_end)/2))
						{
							// create the first node
							// put the original node here
							quadtree[4*j+1].array_num = temp;
							quadtree[4*j+1].mass_center = m[temp];
							quadtree[4*j+1].mass_sum = m[temp];
							quadtree[4*j+1].NW_x = x_start;
							quadtree[4*j+1].NW_y = y_start;
							quadtree[4*j+1].SE_x = (x_start+x_end)/2;
							quadtree[4*j+1].SE_y = (y_start+y_end)/2;
							// create the second node
							quadtree[4*j+2].array_num = -2;
							quadtree[4*j+2].NW_x = x_start;
							quadtree[4*j+2].NW_y = y_start;
							quadtree[4*j+2].SE_x = (x_start+x_end)/2;
							quadtree[4*j+2].SE_y = (y_start+y_end)/2;
							// create the third node
							quadtree[4*j+3].array_num = -2;
							quadtree[4*j+3].NW_x = x_start;
							quadtree[4*j+3].NW_y = y_start;
							quadtree[4*j+3].SE_x = (x_start+x_end)/2;
							quadtree[4*j+3].SE_y = (y_start+y_end)/2;
							// create the fourth node
							quadtree[4*j+4].array_num = -2;
							quadtree[4*j+4].NW_x = x_start;
							quadtree[4*j+4].NW_y = y_start;
							quadtree[4*j+4].SE_x = (x_start+x_end)/2;
							quadtree[4*j+4].SE_y = (y_start+y_end)/2;
						}
						else if ((x[temp]<(x_start+x_end)/2)&&(y[temp]>=(y_start+y_end)/2))
						{
							// create the first node
							// put the original node here
							quadtree[4*j+1].array_num = -2;
							quadtree[4*j+1].NW_x = x_start;
							quadtree[4*j+1].NW_y = (y_start+y_end)/2;
							quadtree[4*j+1].SE_x = (x_start+x_end)/2;
							quadtree[4*j+1].SE_y = y_end;
							// create the second node
							quadtree[4*j+2].array_num = temp;
							quadtree[4*j+2].mass_center = m[temp];
							quadtree[4*j+2].mass_sum = m[temp];
							quadtree[4*j+2].NW_x = x_start;
							quadtree[4*j+2].NW_y = (y_start+y_end)/2;
							quadtree[4*j+2].SE_x = (x_start+x_end)/2;
							quadtree[4*j+2].SE_y = y_end;
							// create the third node
							quadtree[4*j+3].array_num = -2;
							quadtree[4*j+3].NW_x = x_start;
							quadtree[4*j+3].NW_y = (y_start+y_end)/2;
							quadtree[4*j+3].SE_x = (x_start+x_end)/2;
							quadtree[4*j+3].SE_y = y_end;
							// create the fourth node
							quadtree[4*j+4].array_num = -2;
							quadtree[4*j+4].NW_x = x_start;
							quadtree[4*j+4].NW_y = (y_start+y_end)/2;
							quadtree[4*j+4].SE_x = (x_start+x_end)/2;
							quadtree[4*j+4].SE_y = y_end;
						}
						else if ((x[temp]>=(x_start+x_end)/2)&&(y[temp]<(y_start+y_end)/2))
						{
							// create the first node
							// put the original node here
							quadtree[4*j+1].array_num = -2;
							quadtree[4*j+1].NW_x = (x_start+x_end)/2;
							quadtree[4*j+1].NW_y = y_start;
							quadtree[4*j+1].SE_x = x_end;
							quadtree[4*j+1].SE_y = (y_start+y_end)/2;
							// create the second node
							quadtree[4*j+2].array_num = -2;
							quadtree[4*j+2].NW_x = (x_start+x_end)/2;
							quadtree[4*j+2].NW_y = y_start;
							quadtree[4*j+2].SE_x = x_end;
							quadtree[4*j+2].SE_y = (y_start+y_end)/2;
							// create the third node
							quadtree[4*j+3].array_num = temp;
							quadtree[4*j+3].mass_center = m[temp];
							quadtree[4*j+3].mass_sum = m[temp];
							quadtree[4*j+3].NW_x = (x_start+x_end)/2;
							quadtree[4*j+3].NW_y = y_start;
							quadtree[4*j+3].SE_x = x_end;
							quadtree[4*j+3].SE_y = (y_start+y_end)/2;
							// create the fourth node
							quadtree[4*j+4].array_num = -2;
							quadtree[4*j+4].NW_x = (x_start+x_end)/2;
							quadtree[4*j+4].NW_y = y_start;
							quadtree[4*j+4].SE_x = x_end;
							quadtree[4*j+4].SE_y = (y_start+y_end)/2;
						}
						else
						{
							// create the first node
							// put the original node here
							quadtree[4*j+1].array_num = -2;
							quadtree[4*j+1].NW_x = (x_start+x_end)/2;
							quadtree[4*j+1].NW_y = (y_start+y_end)/2;
							quadtree[4*j+1].SE_x = x_end;
							quadtree[4*j+1].SE_y = y_end;
							// create the second node
							quadtree[4*j+2].array_num = -2;
							quadtree[4*j+2].NW_x = (x_start+x_end)/2;
							quadtree[4*j+2].NW_y = (y_start+y_end)/2;
							quadtree[4*j+2].SE_x = x_end;
							quadtree[4*j+2].SE_y = y_end;
							// create the third node
							quadtree[4*j+3].array_num = -2;
							quadtree[4*j+3].NW_x = (x_start+x_end)/2;
							quadtree[4*j+3].NW_y = (y_start+y_end)/2;
							quadtree[4*j+3].SE_x = x_end;
							quadtree[4*j+3].SE_y = y_end;
							// create the fourth node
							quadtree[4*j+4].array_num = temp;
							quadtree[4*j+4].mass_center = m[temp];
							quadtree[4*j+4].mass_sum = m[temp];
							quadtree[4*j+4].NW_x = (x_start+x_end)/2;
							quadtree[4*j+4].NW_y = (y_start+y_end)/2;
							quadtree[4*j+4].SE_x = x_end;
							quadtree[4*j+4].SE_y = y_end;
						}
					}
				}
			}
			else
			{
				j=4*j+3;
				x_start=(x_start+x_end)/2;
				y_start=(y_start+y_end)/2;
				// if the nodes doesn't exist
				// insert the new node.
				if (quadtree[j].array_num==-1)
				{
					// create the first node
					quadtree[j].array_num = i;
					quadtree[j].mass_center = m[i];
					quadtree[j].mass_sum = m[i];
					quadtree[j].NW_x = x_start;
					quadtree[j].NW_y = y_start;
					quadtree[j].SE_x = x_end;
					quadtree[j].SE_y = y_end;
					// create the rest 3 nodes
					quadtree[j-3].array_num = -2;
					quadtree[j-3].NW_x = x_start;
					quadtree[j-3].NW_y = y_start;
					quadtree[j-3].SE_x = x_end;
					quadtree[j-3].SE_y = y_end;

					quadtree[j-2].array_num = -2;
					quadtree[j-2].NW_x = x_start;
					quadtree[j-2].NW_y = y_start;
					quadtree[j-2].SE_x = x_end;
					quadtree[j-2].SE_y = y_end;

					quadtree[j-1].array_num = -2;
					quadtree[j-1].NW_x = x_start;
					quadtree[j-1].NW_y = y_start;
					quadtree[j-1].SE_x = x_end;
					quadtree[j-1].SE_y = y_end;
					break;
				}
				// if the node exist
				// but have not map to a node
				else if (quadtree[j].array_num==-2)
				{
					quadtree[j].array_num = i;
					quadtree[j].mass_center = m[i];
					quadtree[j].mass_sum = m[i];
					quadtree[j].NW_x = x_start;
					quadtree[j].NW_y = y_start;
					quadtree[j].SE_x = x_end;
					quadtree[j].SE_y = y_end;
					break;
				}
				// if the node exist 
				// and have been already mapped to a node
				// move this node to next level
				else
				{	
					// if the node is the leaf
					if (quadtree[4*j+1].array_num==-1)
					{
						int temp = quadtree[j].array_num;
						quadtree[j].array_num = -1;
						
						if ((x[temp]<(x_start+x_end)/2)&&(y[temp]<(y_start+y_end)/2))
						{
							// create the first node
							// put the original node here
							quadtree[4*j+1].array_num = temp;
							quadtree[4*j+1].mass_center = m[temp];
							quadtree[4*j+1].mass_sum = m[temp];
							quadtree[4*j+1].NW_x = x_start;
							quadtree[4*j+1].NW_y = y_start;
							quadtree[4*j+1].SE_x = (x_start+x_end)/2;
							quadtree[4*j+1].SE_y = (y_start+y_end)/2;
							// create the second node
							quadtree[4*j+2].array_num = -2;
							quadtree[4*j+2].NW_x = x_start;
							quadtree[4*j+2].NW_y = y_start;
							quadtree[4*j+2].SE_x = (x_start+x_end)/2;
							quadtree[4*j+2].SE_y = (y_start+y_end)/2;
							// create the third node
							quadtree[4*j+3].array_num = -2;
							quadtree[4*j+3].NW_x = x_start;
							quadtree[4*j+3].NW_y = y_start;
							quadtree[4*j+3].SE_x = (x_start+x_end)/2;
							quadtree[4*j+3].SE_y = (y_start+y_end)/2;
							// create the fourth node
							quadtree[4*j+4].array_num = -2;
							quadtree[4*j+4].NW_x = x_start;
							quadtree[4*j+4].NW_y = y_start;
							quadtree[4*j+4].SE_x = (x_start+x_end)/2;
							quadtree[4*j+4].SE_y = (y_start+y_end)/2;
						}
						else if ((x[temp]<(x_start+x_end)/2)&&(y[temp]>=(y_start+y_end)/2))
						{
							// create the first node
							// put the original node here
							quadtree[4*j+1].array_num = -2;
							quadtree[4*j+1].NW_x = x_start;
							quadtree[4*j+1].NW_y = (y_start+y_end)/2;
							quadtree[4*j+1].SE_x = (x_start+x_end)/2;
							quadtree[4*j+1].SE_y = y_end;
							// create the second node
							quadtree[4*j+2].array_num = temp;
							quadtree[4*j+2].mass_center = m[temp];
							quadtree[4*j+2].mass_sum = m[temp];
							quadtree[4*j+2].NW_x = x_start;
							quadtree[4*j+2].NW_y = (y_start+y_end)/2;
							quadtree[4*j+2].SE_x = (x_start+x_end)/2;
							quadtree[4*j+2].SE_y = y_end;
							// create the third node
							quadtree[4*j+3].array_num = -2;
							quadtree[4*j+3].NW_x = x_start;
							quadtree[4*j+3].NW_y = (y_start+y_end)/2;
							quadtree[4*j+3].SE_x = (x_start+x_end)/2;
							quadtree[4*j+3].SE_y = y_end;
							// create the fourth node
							quadtree[4*j+4].array_num = -2;
							quadtree[4*j+4].NW_x = x_start;
							quadtree[4*j+4].NW_y = (y_start+y_end)/2;
							quadtree[4*j+4].SE_x = (x_start+x_end)/2;
							quadtree[4*j+4].SE_y = y_end;
						}
						else if ((x[temp]>=(x_start+x_end)/2)&&(y[temp]<(y_start+y_end)/2))
						{
							// create the first node
							// put the original node here
							quadtree[4*j+1].array_num = -2;
							quadtree[4*j+1].NW_x = (x_start+x_end)/2;
							quadtree[4*j+1].NW_y = y_start;
							quadtree[4*j+1].SE_x = x_end;
							quadtree[4*j+1].SE_y = (y_start+y_end)/2;
							// create the second node
							quadtree[4*j+2].array_num = -2;
							quadtree[4*j+2].NW_x = (x_start+x_end)/2;
							quadtree[4*j+2].NW_y = y_start;
							quadtree[4*j+2].SE_x = x_end;
							quadtree[4*j+2].SE_y = (y_start+y_end)/2;
							// create the third node
							quadtree[4*j+3].array_num = temp;
							quadtree[4*j+3].mass_center = m[temp];
							quadtree[4*j+3].mass_sum = m[temp];
							quadtree[4*j+3].NW_x = (x_start+x_end)/2;
							quadtree[4*j+3].NW_y = y_start;
							quadtree[4*j+3].SE_x = x_end;
							quadtree[4*j+3].SE_y = (y_start+y_end)/2;
							// create the fourth node
							quadtree[4*j+4].array_num = -2;
							quadtree[4*j+4].NW_x = (x_start+x_end)/2;
							quadtree[4*j+4].NW_y = y_start;
							quadtree[4*j+4].SE_x = x_end;
							quadtree[4*j+4].SE_y = (y_start+y_end)/2;
						}
						else
						{
							// create the first node
							// put the original node here
							quadtree[4*j+1].array_num = -2;
							quadtree[4*j+1].NW_x = (x_start+x_end)/2;
							quadtree[4*j+1].NW_y = (y_start+y_end)/2;
							quadtree[4*j+1].SE_x = x_end;
							quadtree[4*j+1].SE_y = y_end;
							// create the second node
							quadtree[4*j+2].array_num = -2;
							quadtree[4*j+2].NW_x = (x_start+x_end)/2;
							quadtree[4*j+2].NW_y = (y_start+y_end)/2;
							quadtree[4*j+2].SE_x = x_end;
							quadtree[4*j+2].SE_y = y_end;
							// create the third node
							quadtree[4*j+3].array_num = -2;
							quadtree[4*j+3].NW_x = (x_start+x_end)/2;
							quadtree[4*j+3].NW_y = (y_start+y_end)/2;
							quadtree[4*j+3].SE_x = x_end;
							quadtree[4*j+3].SE_y = y_end;
							// create the fourth node
							quadtree[4*j+4].array_num = temp;
							quadtree[4*j+4].mass_center = m[temp];
							quadtree[4*j+4].mass_sum = m[temp];
							quadtree[4*j+4].NW_x = (x_start+x_end)/2;
							quadtree[4*j+4].NW_y = (y_start+y_end)/2;
							quadtree[4*j+4].SE_x = x_end;
							quadtree[4*j+4].SE_y = y_end;
						}
					}
				}	
			}
		}
	}


	//update the mass_sum of from the leaf to the top
/*	for (rit = nodes.rbegin(); rit!=nodes.rend(); ++rit)
	{
		
	}*/

	return 0;

}
