#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <stdlib.h>   

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
		double mass_center_x;
		double mass_center_y;
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
			mass_center_x = -100; 
			mass_center_y = -100;
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

	//test: print the read result
	cout<<"max_x="<<max_x<<", max_y="<<max_y<<endl;
	cout<<"min_x="<<min_x<<", min_y="<<min_y<<endl;

	for (int i=0; i<index; i++)
	{
		cout<<x[i]<<", "<<y[i]<<", "<<m[i]<<", "<<vx[i]<<", "<<vy[i]<<endl;
	} 

	//insert each body to the tree structure
	quadtree[0].array_num = 0;
	quadtree[0].mass_sum = m[0];
	quadtree[0].NW_x = min_x;
	quadtree[0].NW_y = min_y;
	quadtree[0].SE_x = max_x;
	quadtree[0].SE_y = max_y;

	for (int i=1; i<index; i++)
	{
		double x_start = min_x;
		double x_end = max_x;
		double y_start = min_y;
		double y_end = max_y;
		int j=0;
		int test_int=0;
		while (quadtree[j].array_num!=-2)
		{
			cout<<test_int++<<endl;
			cout<<"i="<<i<<", j="<<j<<" ,"<<quadtree[j].array_num<<endl;
			// if there is a hole exist
			if (quadtree[j].array_num == -3)
			{
				if ((x[i]<(x_start+x_end)/2)&&(y[i]<(y_start+y_end)/2))
				{
					j=4*j+1;
					x_end = (x_start+x_end)/2;
					y_end = (y_start+y_end)/2;
				}
				else if ((x[i]<(x_start+x_end)/2)&&(y[i]>=(y_start+y_end)/2))
				{
					j=4*j+2;
					x_end = (x_start+x_end)/2;
					y_start = (y_start+y_end)/2;
				}
				else if ((x[i]>=(x_start+x_end)/2)&&(y[i]<(y_start+y_end)/2))
				{
					j=4*j+3;
					x_start = (x_start+x_end)/2;
					y_end = (y_start+y_end)/2;
				}
				else if ((x[i]>=(x_start+x_end)/2)&&(y[i]>=(y_start+y_end)/2))
				{
					j=4*j+4;
					x_start = (x_start+x_end)/2;
					y_start = (y_start+y_end)/2;
				}
			}
			// if there is a node exist
			else if (quadtree[j].array_num>-1)
			{
				cout<<"there is a node!"<<endl;
				int temp = quadtree[j].array_num;
				quadtree[j].array_num = -3;
				quadtree[j].mass_sum = -100;
				// insert current node to next level
				if ((x[temp]<(x_start+x_end)/2)&&(y[temp]<(y_start+y_end)/2))
				{
					cout<<"insert in NW"<<endl;
					// create the first node
					quadtree[4*j+1].array_num = temp;
					quadtree[4*j+1].mass_sum = m[temp];
					quadtree[4*j+1].mass_center_x = x[temp]; 
					quadtree[4*j+1].mass_center_y = y[temp];
					quadtree[4*j+1].NW_x = x_start;
					quadtree[4*j+1].NW_y = y_start;
					quadtree[4*j+1].SE_x = (x_start+x_end)/2;
					quadtree[4*j+1].SE_y = (y_start+y_end)/2;
					// create the rest 3 nodes
					quadtree[4*j+2].array_num = -2;
					quadtree[4*j+2].NW_x = x_start;
					quadtree[4*j+2].NW_y = y_start;
					quadtree[4*j+2].SE_x = (x_start+x_end)/2;
					quadtree[4*j+2].SE_y = (y_start+y_end)/2;
				
					quadtree[4*j+3].array_num = -2;
					quadtree[4*j+3].NW_x = x_start;
					quadtree[4*j+3].NW_y = y_start;
					quadtree[4*j+3].SE_x = (x_start+x_end)/2;
					quadtree[4*j+3].SE_y = (y_start+y_end)/2;
	
					quadtree[4*j+4].array_num = -2;
					quadtree[4*j+4].NW_x = x_start;
					quadtree[4*j+4].NW_y = y_start;
					quadtree[4*j+4].SE_x = (x_start+x_end)/2;
					quadtree[4*j+4].SE_y = (y_start+y_end)/2;
				}
				else if ((x[temp]<(x_start+x_end)/2)&&(y[temp]>=(y_start+y_end)/2))
				{
					cout<<"insert in SW"<<endl;
					// create the first node
					quadtree[4*j+1].array_num = -2;
					quadtree[4*j+1].NW_x = x_start;
					quadtree[4*j+1].NW_y = (y_start+y_end)/2;
					quadtree[4*j+1].SE_x = (x_start+x_end)/2;
					quadtree[4*j+1].SE_y = y_end;
					// create the rest 3 nodes
					quadtree[4*j+2].array_num = temp;
					quadtree[4*j+2].mass_sum = m[temp];
					quadtree[4*j+2].mass_center_x = x[temp]; 
					quadtree[4*j+2].mass_center_y = y[temp];
					quadtree[4*j+2].NW_x = x_start;
					quadtree[4*j+2].NW_y = (y_start+y_end)/2;
					quadtree[4*j+2].SE_x = (x_start+x_end)/2;
					quadtree[4*j+2].SE_y = y_end;
				
					quadtree[4*j+3].array_num = -2;
					quadtree[4*j+3].NW_x = x_start;
					quadtree[4*j+3].NW_y = (y_start+y_end)/2;
					quadtree[4*j+3].SE_x = (x_start+x_end)/2;
					quadtree[4*j+3].SE_y = y_end;
	
					quadtree[4*j+4].array_num = -2;
					quadtree[4*j+4].NW_x = x_start;
					quadtree[4*j+4].NW_y = (y_start+y_end)/2;
					quadtree[4*j+4].SE_x = (x_start+x_end)/2;
					quadtree[4*j+4].SE_y = y_end;
				}
				else if ((x[temp]>=(x_start+x_end)/2)&&(y[temp]<(y_start+y_end)/2))
				{
					cout<<"insert in NE"<<endl;
					// create the first node
					quadtree[4*j+1].array_num = -2;
					quadtree[4*j+1].NW_x = (x_start+x_end)/2;
					quadtree[4*j+1].NW_y = y_start;
					quadtree[4*j+1].SE_x = x_end;
					quadtree[4*j+1].SE_y = (y_start+y_end)/2;
					// create the rest 3 nodes
					quadtree[4*j+2].array_num = -2;
					quadtree[4*j+2].NW_x = (x_start+x_end)/2;
					quadtree[4*j+2].NW_y = y_start;
					quadtree[4*j+2].SE_x = x_end;
					quadtree[4*j+2].SE_y = (y_start+y_end)/2;

					quadtree[4*j+3].array_num = temp;
					quadtree[4*j+3].mass_sum = m[temp];
					quadtree[4*j+3].mass_center_x = x[temp]; 
					quadtree[4*j+3].mass_center_y = y[temp];
					quadtree[4*j+3].NW_x = (x_start+x_end)/2;
					quadtree[4*j+3].NW_y = y_start;
					quadtree[4*j+3].SE_x = x_end;
					quadtree[4*j+3].SE_y = (y_start+y_end)/2;
	
					quadtree[4*j+4].array_num = -2;
					quadtree[4*j+4].NW_x = (x_start+x_end)/2;
					quadtree[4*j+4].NW_y = y_start;
					quadtree[4*j+4].SE_x = x_end;
					quadtree[4*j+4].SE_y = (y_start+y_end)/2;
				}
				else if ((x[temp]>=(x_start+x_end)/2)&&(y[temp]>=(y_start+y_end)/2))
				{
					cout<<"insert in SE"<<endl;
					// create the first node
					quadtree[4*j+1].array_num = -2;
					quadtree[4*j+1].NW_x = (x_start+x_end)/2;
					quadtree[4*j+1].NW_y = (y_start+y_end)/2;
					quadtree[4*j+1].SE_x = x_end;
					quadtree[4*j+1].SE_y = y_end;
					// create the rest 3 nodes
					quadtree[4*j+2].array_num = -2;
					quadtree[4*j+2].NW_x = (x_start+x_end)/2;
					quadtree[4*j+2].NW_y = (y_start+y_end)/2;
					quadtree[4*j+2].SE_x = x_end;
					quadtree[4*j+2].SE_y = y_end;
				
					quadtree[4*j+3].array_num = -2;
					quadtree[4*j+3].NW_x = (x_start+x_end)/2;
					quadtree[4*j+3].NW_y = (y_start+y_end)/2;
					quadtree[4*j+3].SE_x = x_end;
					quadtree[4*j+3].SE_y = y_end;
	
					quadtree[4*j+4].array_num = temp;
					quadtree[4*j+4].mass_sum = m[temp];
					quadtree[4*j+4].mass_center_x = x[temp]; 
					quadtree[4*j+4].mass_center_y = y[temp];
					quadtree[4*j+4].NW_x = (x_start+x_end)/2;
					quadtree[4*j+4].NW_y = (y_start+y_end)/2;
					quadtree[4*j+4].SE_x = x_end;
					quadtree[4*j+4].SE_y = y_end;
				}
			
				if ((x[i]<(x_start+x_end)/2)&&(y[i]<(y_start+y_end)/2))
				{
					j=4*j+1;
					x_end = (x_start+x_end)/2;
					y_end = (y_start+y_end)/2;
				}
				else if ((x[i]<(x_start+x_end)/2)&&(y[i]>=(y_start+y_end)/2))
				{
					j=4*j+2;
					x_end = (x_start+x_end)/2;
					y_start = (y_start+y_end)/2;
				}
				else if ((x[i]>=(x_start+x_end)/2)&&(y[i]<(y_start+y_end)/2))
				{
					j=4*j+3;
					x_start = (x_start+x_end)/2;
					y_end = (y_start+y_end)/2;
				}
				else if ((x[i]>=(x_start+x_end)/2)&&(y[i]>=(y_start+y_end)/2))
				{
					j=4*j+4;
					x_start = (x_start+x_end)/2;
					y_start = (y_start+y_end)/2;
				}
			}
		}
		if (quadtree[j].array_num == -2)
		{
			quadtree[j].array_num = i;
			quadtree[j].mass_sum = m[i];
			quadtree[j].mass_center_x = x[i];
			quadtree[j].mass_center_y = y[i];
		}
	}
	// for test
	cout<<"print out the insertion result:"<<endl;
	for (int i=0; i<4*size; i++)
	{
		if (quadtree[i].array_num!=-1)
		{
			cout<<"cellnum["<<i<<"] = "<<quadtree[i].array_num<<", mass_sum = "<<quadtree[i].mass_sum
			<<", NW = ("<<quadtree[i].NW_x<<", "<<quadtree[i].NW_y<<"), SE = ("<<quadtree[i].SE_x<<", "<<quadtree[i].SE_y<<")"<<endl;
		}
	}
	cout<<endl;

	//update the mass_sum of from the leaf to the top
	for (int i=4*size-1; i>-1; i--)
	{
		// if this is a internal node
		// we need to update the mass center
		if ((quadtree[i].array_num==-3)&&(quadtree[i].mass_sum<0))
		{
			//first we need to figure out whether the node is ready to compute 
			//the mass center	
			bool IsReady = true;
			double temp_mass_sum = 0;
			double temp_mass_center_x = 0;
			double temp_mass_center_y = 0;
			for (int j=1; j<5; j++)
			{
				if ((quadtree[4*i+j].array_num>-1)||(quadtree[4*i+j].array_num==-3))
				{
					if (quadtree[4*i+j].mass_sum==-100)
					{
						IsReady = false;
					}
				}
			}
			if (IsReady)
			{
				for (int j=1; j<5; j++)
				{
					if (quadtree[4*i+j].mass_sum>0)
					{
						int temp_index = quadtree[4*i+j].array_num;
						temp_mass_sum = temp_mass_sum+quadtree[4*i+j].mass_sum;
						temp_mass_center_x = temp_mass_center_x + quadtree[4*i+j].mass_sum * x[temp_index];
						temp_mass_center_y = temp_mass_center_y + quadtree[4*i+j].mass_sum * y[temp_index];
					}
				}
				temp_mass_center_x = temp_mass_center_x / temp_mass_sum;
				temp_mass_center_y = temp_mass_center_y / temp_mass_sum;

				quadtree[i].mass_sum = temp_mass_sum;
				quadtree[i].mass_center_x = temp_mass_center_x;
				quadtree[i].mass_center_y = temp_mass_center_y;
			}
			
		}
	}

	// for test
	cout<<"print out the result of the computation of the mass center."<<endl;
	for (int i=0; i<4*size; i++)
	{
		if (quadtree[i].array_num!=-1)
		{
			cout<<"cellnum["<<i<<"] = "<<quadtree[i].array_num<<", mass_sum = "<<quadtree[i].mass_sum
			<<", mass(x,y) = ("<<quadtree[i].mass_center_x<<", "<<quadtree[i].mass_center_y<<")"<<endl;
		}
	}
	cout<<endl;

	return 0;

}
