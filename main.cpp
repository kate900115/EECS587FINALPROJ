#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cmath>
#include <map>
#include <stdlib.h>   

#define size 500
#define TreeSize 2000
#define MAX 10000000
#define MIN -10000000
#define theta 0.5
//#define G 6.67e-11
#define G 6.67
#define T 0.01


using namespace std;

double max_x;
double min_x;
double max_y;
double min_y;

class body
{
	public:
		// the coordinate of the mass center
		double mass_center_x;
		double mass_center_y;
		// the sum of the mass of the sub node
		double mass_sum;
		// array index that store the data
		double array_num;
		// the square space 
		double NW_x;
		double NW_y;
		double SE_x;
		double SE_y;
		// the force
		double Fx;
		double Fy;
		//
		int tree_idx;
		
		//constructor
		//mass initialized as -100 indicate the result of the center of mass is not computed
		//array_num initialized as -1 indicate it don't contain any body
		//array_num = -2 indicate it contains an empty body
		//array_num = -3 indicate it contains a body which has children
		//array_num >=0 indicate it contains a leaf node
		
		body()
		{
			// the coordinate of the mass center
			mass_center_x = -100; 
			mass_center_y = -100;
			// the sum of the mass of the sub node
			mass_sum = -100; 
			// array index that store the data
			array_num = -1;
			// tree index
			tree_idx = -1;
			// the square space 
			NW_x = min_x;
			NW_y = max_y;
			SE_x = max_x;
			SE_y = min_y;
			// the force 
			Fx = 0;
			Fy = 0;
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
	
	// force
	double fx[size];
	double fy[size];

	// index in quadtree
	int idx[size];

	// to store the quadtree information
	body quadtree[size*4];
	
	//map<int, body> nodes;
	//map<int, body>::iterator it;
	//map<int, body>::reverse_iterator rit;
	
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
		fx[index] = 0;
		fy[index] = 0;
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

	for (int i=0; i<TreeSize; i++)
	{
		quadtree[i].tree_idx = i;
	}

	//insert each body to the tree structure
	quadtree[0].array_num = 0;
	quadtree[0].mass_sum = m[0];
	quadtree[0].NW_x = min_x;
	quadtree[0].NW_y = min_y;
	quadtree[0].SE_x = max_x;
	quadtree[0].SE_y = max_y;
	idx[0]=0;

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
					idx[temp] = 4*j+1;
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
					idx[temp] = 4*j+2;
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
					idx[temp] = 4*j+3;
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
					idx[temp] = 4*j+4;
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
			idx[i]=j;
		}
	}
	// for test the insertion of each nodes
	cout<<"print out the insertion result:"<<endl;
	for (int i=0; i<TreeSize; i++)
	{
		if (quadtree[i].array_num!=-1)
		{
			cout<<"cellnum["<<i<<"] = "<<quadtree[i].array_num<<", mass_sum = "<<quadtree[i].mass_sum
			<<", NW = ("<<quadtree[i].NW_x<<", "<<quadtree[i].NW_y<<"), SE = ("<<quadtree[i].SE_x<<", "<<quadtree[i].SE_y<<")"<<endl;
		}
	}
	cout<<endl;
	cout<<"the index array is:"<<endl;
	for (int i=0; i<index; i++)
	{
		cout<<"index["<<i<<"]="<<idx[i]<<endl;
	}

	//update the mass_sum of from the leaf to the top
	for (int i=TreeSize-1; i>-1; i--)
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
						temp_mass_sum = temp_mass_sum+quadtree[4*i+j].mass_sum;
						temp_mass_center_x = temp_mass_center_x + quadtree[4*i+j].mass_sum * quadtree[4*i+j].mass_center_x;
						temp_mass_center_y = temp_mass_center_y + quadtree[4*i+j].mass_sum * quadtree[4*i+j].mass_center_y;
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

	// for test the computation result of mass center of each nodes
	cout<<"print out the result of the computation of the mass center."<<endl;
	for (int i=0; i<TreeSize; i++)
	{
		if (quadtree[i].array_num!=-1)
		{
			cout<<"cellnum["<<i<<"] = "<<quadtree[i].array_num<<", mass_sum = "<<quadtree[i].mass_sum
			<<", mass(x,y) = ("<<quadtree[i].mass_center_x<<", "<<quadtree[i].mass_center_y<<")"<<endl;
		}
	}
	cout<<endl;


	// approximately sort the bodies by spacial distance

	// compute forces acting on each body
	for (int i=0; i<index; i++)
	{
		//int TreeIdx = idx[i];
		// traverse from the root of the quadtree
		// GPU cannot use recursive
		// so we create a stack
		body stack[TreeSize];
		int tail=0;
		stack[0]=quadtree[0];
		tail++;

		while (tail!=0)
		{
			// array_num=-2 meaning that the node is a hole and have no child node
			// delete the node
			if (stack[tail-1].array_num==-2)
			{
				tail--;
			}

			// array_num=-3 meaning that the node have child nodes
			// put all the child node into stack
			else if (stack[tail-1].array_num==-3)
			{
				double s = (stack[tail-1].SE_x - stack[tail-1].NW_x)>(stack[tail-1].SE_y - stack[tail-1].NW_y)? 
			    	           (stack[tail-1].SE_x - stack[tail-1].NW_x):(stack[tail-1].SE_y - stack[tail-1].NW_y);
			
				// distance between the current node and the mass center of the node
				double d = sqrt((x[i]-stack[tail-1].mass_center_x)*(x[i]-stack[tail-1].mass_center_x)
			 	           +(y[i]-stack[tail-1].mass_center_y)*(y[i]-stack[tail-1].mass_center_y));
				cout<<"i = "<<i<<", d= "<<d<<endl;

	
				if (s/d<theta)
				{
					int RealIdx = idx[i];

					//compute the force
					double Forth_x = G * m[i]* stack[tail-1].mass_sum * (x[i]-stack[tail-1].mass_center_x) / (d*d*d); 	
					double Forth_y = G * m[i]* stack[tail-1].mass_sum * (y[i]-stack[tail-1].mass_center_y) / (d*d*d);
					cout<<"index = "<<RealIdx<<", Fx = "<<Forth_x<<", Fy = "<<Forth_y<<endl;
					
					quadtree[RealIdx].Fx = quadtree[RealIdx].Fx + Forth_x;
					quadtree[RealIdx].Fy = quadtree[RealIdx].Fy + Forth_y;
					fx[i] = fx[i] + Forth_x;
					fy[i] = fy[i] + Forth_y;
					tail--;
				}
				else
				{
					int new_index = stack[tail-1].tree_idx;
					stack[tail-1]=quadtree[4*new_index+1];
					stack[tail]=quadtree[4*new_index+2];
					stack[tail+1]=quadtree[4*new_index+3];
					stack[tail+2]=quadtree[4*new_index+4];
					tail=tail+3;
				}
			} 
			else if (stack[tail-1].array_num>-1)
			{
				// compute the force directly
				double d = sqrt((x[i]-stack[tail-1].mass_center_x)*(x[i]-stack[tail-1].mass_center_x)
			 	           +(y[i]-stack[tail-1].mass_center_y)*(y[i]-stack[tail-1].mass_center_y));

				int RealIdx = idx[i];
				
				if (i!=stack[tail-1].array_num)
				{

					double Forth_x = G * m[i]* stack[tail-1].mass_sum * (x[i]-stack[tail-1].mass_center_x) / (d*d*d); 	
					double Forth_y = G * m[i]* stack[tail-1].mass_sum * (y[i]-stack[tail-1].mass_center_y) / (d*d*d);

					cout<<"index = "<<index<<", Fx = "<<Forth_x<<", Fy = "<<Forth_y<<endl;

					quadtree[RealIdx].Fx = quadtree[RealIdx].Fx + Forth_x;
					quadtree[RealIdx].Fy = quadtree[RealIdx].Fy + Forth_y;
					fx[i] = fx[i] + Forth_x;
					fy[i] = fy[i] + Forth_y;
				}

				tail--;
			}
		}
	}

	// for test the computation result of mass center of each nodes
	cout<<"print out the result of the computation of the mass center."<<endl;
	for (int i=0; i<TreeSize; i++)
	{
		if (quadtree[i].array_num!=-1)
		{
			cout<<"cellnum["<<i<<"] = "<<quadtree[i].array_num<<", F(x,y) = ("<<quadtree[i].Fx<<", "<<quadtree[i].Fy<<")"<<endl;
		}
	}
	cout<<endl;
	
	// update body position and velocities
	for (int i=0; i<size; i++)
	{
		double temp_vx = vx[i] + m[i]/fx[i] * T;
		double temp_vy = vy[i] + m[i]/fy[i] * T;
		double temp_x = x[i] + vx[i]*T + 0.5* m[i]/fx[i]*T*T;
		double temp_y = y[i] + vy[i]*T + 0.5* m[i]/fy[i]*T*T;
		vx[i]= temp_vx;
		vy[i]= temp_vy;
		x[i] = temp_x;
		y[i] = temp_y;
	}
	

	return 0;

}
