#include <iostream>
#include <fstream>
#include <sstream>
#include <stack>
#include <string>
#include <cmath>
#include <map>
#include <stdlib.h> 
#include <omp.h>  

//#define StarNum 50000
#define MAX 10000000
#define MIN -10000000
#define theta 0.5
#define G 6.67
#define T 0.01
//#define thread_num 7


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
		bool IsReady;
		bool working;
		bool moving;
		
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

			IsReady = false;
			working = false;
			moving = false;
		}
};



int main(int argc, char** argv)
{
	cout.precision(10);
	// the max coordinate and the minimum coordinate of 
	// the 2D-space
	double max_x=MIN;
	double min_x=MAX;
	double max_y=MIN;
	double min_y=MAX;
	string FileName = string(argv[1]);
	ifstream InFile;
	string num = string(argv[2]);
	int StarNum = atoi(num.c_str());
	string thnum = string(argv[3]);
	int thread_num = atoi(thnum.c_str());
	InFile.open(FileName.c_str());
	
	// coordinate
	double x[StarNum];
	double y[StarNum];
	// mass
	double m[StarNum];
	
	// speed
	double vx[StarNum];
	double vy[StarNum];
	
	// force
	double fx[StarNum];
	double fy[StarNum];

	// index in quadtree
	int idx[StarNum];

	// to store the quadtree information
	//body quadtree[size*4];
	
	map<int, body> quadtree;
	map<int, body>::iterator it;
	//map<int, body>::reverse_iterator rit;
	
	string line;
	int index = 0;
	stack<body*> UncomputedBody;

	long mass_all=0;
	
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
		mass_all+=m[index];
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
//	cout<<"max_x="<<max_x<<", max_y="<<max_y<<endl;
//	cout<<"min_x="<<min_x<<", min_y="<<min_y<<endl;

//	for (int i=0; i<index; i++)
//	{
//		cout<<x[i]<<", "<<y[i]<<", "<<m[i]<<", "<<vx[i]<<", "<<vy[i]<<endl;
//	} 

	double begin_time = omp_get_wtime();

	//insert each body to the tree structure
	quadtree[0].array_num = 0;
	quadtree[0].mass_sum = m[0];
	quadtree[0].NW_x = min_x;
	quadtree[0].NW_y = min_y;
	quadtree[0].SE_x = max_x;
	quadtree[0].SE_y = max_y;
	quadtree[0].tree_idx = 0;
	idx[0]=0;


	//#pragma omp parallel for num_threads(thread_num)
	for (int i=1; i<index/10; i++)
	{
		double x_start = min_x;
		double x_end = max_x;
		double y_start = min_y;
		double y_end = max_y;
		int j=0;

		
		while (quadtree[j].array_num!=-2)
		{

			//cout<<"i="<<i<<", j="<<j<<" ,"<<quadtree[j].array_num<<endl;
			
			// if there is a node exist
			//#pragma omp critical(update_quadtree)
			if (quadtree[j].array_num>-1)
			{
				//cout<<"there is a node!"<<endl;		
				int temp = quadtree[j].array_num;
				quadtree[j].array_num = -3;
				quadtree[j].mass_sum = -100;
				// insert current node to next level
				if ((x[temp]<(x_start+x_end)/2)&&(y[temp]<(y_start+y_end)/2))
				{
					//cout<<"insert in NW"<<endl;
					// create the first node
					body NW;
					
					NW.array_num = temp;
					NW.mass_sum = m[temp];
					NW.mass_center_x = x[temp]; 
					NW.mass_center_y = y[temp];
					NW.tree_idx = 4*j+1;
					idx[temp] = 4*j+1;
					NW.NW_x = x_start;
					NW.NW_y = y_start;
					NW.SE_x = (x_start+x_end)/2;
					NW.SE_y = (y_start+y_end)/2;
					quadtree[4*j+1]=NW;
					// create the rest 3 nodes
					body SW;
					SW.tree_idx = 4*j+2;
					SW.array_num = -2;
					SW.NW_x = x_start;
					SW.NW_y = (y_start+y_end)/2;
					SW.SE_x = (x_start+x_end)/2;
					SW.SE_y = y_end;
					quadtree[4*j+2]=SW;
					
					body NE;
					NE.tree_idx = 4*j+3;
					NE.array_num = -2;
					NE.NW_x = (x_start+x_end)/2;
					NE.NW_y = y_start;
					NE.SE_x = x_end;
					NE.SE_y = (y_start+y_end)/2;
					quadtree[4*j+3]=NE;
		
					body SE;
					SE.tree_idx = 4*j+4;
					SE.array_num = -2;
					SE.NW_x = (x_start+x_end)/2;
					SE.NW_y = (y_start+y_end)/2;
					SE.SE_x = x_end;
					SE.SE_y = y_end;
					quadtree[4*j+4]=SE;
				}
				else if ((x[temp]<(x_start+x_end)/2)&&(y[temp]>=(y_start+y_end)/2))
				{
					//cout<<"insert in SW"<<endl;
					// create the first node
					body NW;
					NW.tree_idx = 4*j+1;
					NW.array_num = -2;
					NW.NW_x = x_start;
					NW.NW_y = y_start;
					NW.SE_x = (x_start+x_end)/2;
					NW.SE_y = (y_start+y_end)/2;
					quadtree[4*j+1]=NW;
					// create the rest 3 nodes
					body SW;
					SW.tree_idx = 4*j+2;
					SW.array_num = temp;
					SW.mass_sum = m[temp];
					SW.mass_center_x = x[temp]; 
					SW.mass_center_y = y[temp];
					idx[temp] = 4*j+2;
					SW.NW_x = x_start;
					SW.NW_y = (y_start+y_end)/2;
					SW.SE_x = (x_start+x_end)/2;
					SW.SE_y = y_end;
					quadtree[4*j+2]=SW;
					
					body NE;
					NE.tree_idx = 4*j+3;
					NE.array_num = -2;
					NE.NW_x = (x_start+x_end)/2;
					NE.NW_y = y_start;
					NE.SE_x = x_end;
					NE.SE_y = (y_start+y_end)/2;
					quadtree[4*j+3]=NE;
		
					body SE;
					SE.tree_idx = 4*j+4;
					SE.array_num = -2;
					SE.NW_x = (x_start+x_end)/2;
					SE.NW_y = (y_start+y_end)/2;
					SE.SE_x = x_end;
					SE.SE_y = y_end;
					quadtree[4*j+4]=SE;
				}
				else if ((x[temp]>=(x_start+x_end)/2)&&(y[temp]<(y_start+y_end)/2))
				{
					//cout<<"insert in NE"<<endl;
					// create the first node
					body NW;
					NW.tree_idx = 4*j+1;
					NW.array_num = -2;
					NW.NW_x = x_start;
					NW.NW_y = y_start;
					NW.SE_x = (x_start+x_end)/2;
					NW.SE_y = (y_start+y_end)/2;
					quadtree[4*j+1]=NW;
					// create the rest 3 nodes
					body SW;
					SW.tree_idx = 4*j+2;
					SW.array_num = -2;
					SW.NW_x = x_start;
					SW.NW_y = (y_start+y_end)/2;
					SW.SE_x = (x_start+x_end)/2;
					SW.SE_y = y_end;
					quadtree[4*j+2]=SW;
		
					body NE;
					NE.tree_idx = 4*j+3;
					NE.array_num = temp;
					NE.mass_sum = m[temp];
					NE.mass_center_x = x[temp]; 
					NE.mass_center_y = y[temp];
					idx[temp] = 4*j+3;
					NE.NW_x = (x_start+x_end)/2;
					NE.NW_y = y_start;
					NE.SE_x = x_end;
					NE.SE_y = (y_start+y_end)/2;
					quadtree[4*j+3]=NE;	
					
					body SE;
					SE.tree_idx = 4*j+4;
					SE.array_num = -2;
					SE.NW_x = (x_start+x_end)/2;
					SE.NW_y = (y_start+y_end)/2;
					SE.SE_x = x_end;
					SE.SE_y = y_end;
					quadtree[4*j+4]=SE;
				}
				else if ((x[temp]>=(x_start+x_end)/2)&&(y[temp]>=(y_start+y_end)/2))
				{	
					//cout<<"insert in SE"<<endl;
					// create the first node
					body NW;
					NW.tree_idx = 4*j+1;
					NW.array_num = -2;
					NW.NW_x = x_start;
					NW.NW_y = y_start;
					NW.SE_x = (x_start+x_end)/2;
					NW.SE_y = (y_start+y_end)/2;
					quadtree[4*j+1]=NW;
					// create the rest 3 nodes
					body SW;
					SW.tree_idx = 4*j+2;
					SW.array_num = -2;
					SW.NW_x = x_start;
					SW.NW_y = (y_start+y_end)/2;
					SW.SE_x = (x_start+x_end)/2;
					SW.SE_y = y_end;
					quadtree[4*j+2]=SW;
					
					body NE;
					NE.tree_idx = 4*j+3;
					NE.array_num = -2;
					NE.NW_x = (x_start+x_end)/2;
					NE.NW_y = y_start;
					NE.SE_x = x_end;
					NE.SE_y = (y_start+y_end)/2;
					quadtree[4*j+3]=NE;
						
					body SE;
					SE.tree_idx = 4*j+4;
					SE.array_num = temp;
					SE.mass_sum = m[temp];
					SE.mass_center_x = x[temp]; 
					SE.mass_center_y = y[temp];
					idx[temp] = 4*j+4;
					SE.NW_x = (x_start+x_end)/2;
					SE.NW_y = (y_start+y_end)/2;
					SE.SE_x = x_end;
					SE.SE_y = y_end;
					quadtree[4*j+4]=SE;	
				}
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
		//#pragma omp critical(update_quadtree)
		if (quadtree[j].array_num == -2)
		{
			body NewBody;
			NewBody.array_num = i;
			NewBody.mass_sum = m[i];
			NewBody.mass_center_x = x[i];
			NewBody.mass_center_y = y[i];
			NewBody.tree_idx = j;
			NewBody.NW_x = quadtree[j].NW_x;
			NewBody.NW_y = quadtree[j].NW_y;
			NewBody.SE_x = quadtree[j].SE_x;
			NewBody.SE_y = quadtree[j].SE_y;
			idx[i]=j;
			quadtree[j]=NewBody;
		}
	}

	double part1_time = omp_get_wtime();

	#pragma omp parallel for num_threads(thread_num)
	for (long i=index/10; i<index; i++)
	{
		double x_start = min_x;
		double x_end = max_x;
		double y_start = min_y;
		double y_end = max_y;
		long j=0;
		bool spin_flag = false;
				
		while ((quadtree[j].array_num!=-2))//&&(quadtree[j].array_num!=-1))
		{	
			
			//#pragma omp critical(cout)
			//cout<<"i="<<i<<", j="<<j<<" ,"<<quadtree[j].array_num<<", "<<quadtree[j].moving<<endl;
			//if there is a node exist

			#pragma omp critical(updatemoving)
			{
				if ((quadtree[j].array_num==-3)&&(quadtree[j].moving == false))
				{
					spin_flag = false;
				}
				else if ((quadtree[j].array_num>-1)&&(quadtree[j].moving == false))
				{
					//#pragma omp critical(cout)
					//cout<<"to insert node "<<i<<", we need quad "<<j<<" to move"<<endl;
					quadtree[j].moving = true;
				}
				//else if((quadtree[j].array_num>-1)&&(quadtree[j].moving == true))
				else if(quadtree[j].moving == true)				
				{
					//#pragma omp critical(cout)
					//cout<<"node "<<i<<" need to wait at quad "<<j<<endl;
					spin_flag = true;
				}		
			}
						
			if (!spin_flag)
			{
				if (quadtree[j].moving == true)
				{
					//#pragma omp critical(cout)
					//cout<<"when inserting "<<i<<" we find there is a node! quad "<<j<<endl;		
					long temp = quadtree[j].array_num;
					quadtree[j].array_num = -3;
					quadtree[j].mass_sum = -100;
					// insert current node to next level
					#pragma omp critical(createnodes)
					if ((x[temp]<(x_start+x_end)/2)&&(y[temp]<(y_start+y_end)/2))
					{
						//#pragma omp critical(cout)
						//cout<<temp<<" reinsert in NW"<<endl;
						// create the first node
						body NW;
						
						NW.array_num = temp;
						NW.mass_sum = m[temp];
						NW.mass_center_x = x[temp]; 
						NW.mass_center_y = y[temp];
						NW.tree_idx = 4*j+1;
						idx[temp] = 4*j+1;
						NW.NW_x = x_start;
						NW.NW_y = y_start;
						NW.SE_x = (x_start+x_end)/2;
						NW.SE_y = (y_start+y_end)/2;
						quadtree[4*j+1]=NW;
						// create the rest 3 nodes
						body SW;
						SW.tree_idx = 4*j+2;
						SW.array_num = -2;
						SW.NW_x = x_start;
						SW.NW_y = (y_start+y_end)/2;
						SW.SE_x = (x_start+x_end)/2;
						SW.SE_y = y_end;
						quadtree[4*j+2]=SW;
				
						body NE;
						NE.tree_idx = 4*j+3;
						NE.array_num = -2;
						NE.NW_x = (x_start+x_end)/2;
						NE.NW_y = y_start;
						NE.SE_x = x_end;
						NE.SE_y = (y_start+y_end)/2;
						quadtree[4*j+3]=NE;
			
						body SE;
						SE.tree_idx = 4*j+4;
						SE.array_num = -2;
						SE.NW_x = (x_start+x_end)/2;
						SE.NW_y = (y_start+y_end)/2;
						SE.SE_x = x_end;
						SE.SE_y = y_end;
						quadtree[4*j+4]=SE;
					}
					else if ((x[temp]<(x_start+x_end)/2)&&(y[temp]>=(y_start+y_end)/2))
					{
						//#pragma omp critical(cout)
						//cout<<temp<<" reinsert in SW"<<endl;
						// create the first node
						body NW;
						NW.tree_idx = 4*j+1;
						NW.array_num = -2;
						NW.NW_x = x_start;
						NW.NW_y = y_start;
						NW.SE_x = (x_start+x_end)/2;
						NW.SE_y = (y_start+y_end)/2;
						quadtree[4*j+1]=NW;
						// create the rest 3 nodes
						body SW;
						SW.tree_idx = 4*j+2;
						SW.array_num = temp;
						SW.mass_sum = m[temp];
						SW.mass_center_x = x[temp]; 
						SW.mass_center_y = y[temp];
						idx[temp] = 4*j+2;
						SW.NW_x = x_start;
						SW.NW_y = (y_start+y_end)/2;
						SW.SE_x = (x_start+x_end)/2;
						SW.SE_y = y_end;
						quadtree[4*j+2]=SW;
				
						body NE;
						NE.tree_idx = 4*j+3;
						NE.array_num = -2;
						NE.NW_x = (x_start+x_end)/2;
						NE.NW_y = y_start;
						NE.SE_x = x_end;
						NE.SE_y = (y_start+y_end)/2;
						quadtree[4*j+3]=NE;
		
						body SE;
						SE.tree_idx = 4*j+4;
						SE.array_num = -2;
						SE.NW_x = (x_start+x_end)/2;
						SE.NW_y = (y_start+y_end)/2;
						SE.SE_x = x_end;
						SE.SE_y = y_end;
						quadtree[4*j+4]=SE;
					}
					else if ((x[temp]>=(x_start+x_end)/2)&&(y[temp]<(y_start+y_end)/2))
					{
						//#pragma omp critical(cout)
						//cout<<temp<<" reinsert in NE"<<endl;
						// create the first node
						body NW;
						NW.tree_idx = 4*j+1;
						NW.array_num = -2;
						NW.NW_x = x_start;
						NW.NW_y = y_start;
						NW.SE_x = (x_start+x_end)/2;
						NW.SE_y = (y_start+y_end)/2;
						quadtree[4*j+1]=NW;
						// create the rest 3 nodes
						body SW;
						SW.tree_idx = 4*j+2;
						SW.array_num = -2;
						SW.NW_x = x_start;
						SW.NW_y = (y_start+y_end)/2;
						SW.SE_x = (x_start+x_end)/2;
						SW.SE_y = y_end;
						quadtree[4*j+2]=SW;
		
						body NE;
						NE.tree_idx = 4*j+3;
						NE.array_num = temp;
						NE.mass_sum = m[temp];
						NE.mass_center_x = x[temp]; 
						NE.mass_center_y = y[temp];
						idx[temp] = 4*j+3;
						NE.NW_x = (x_start+x_end)/2;
						NE.NW_y = y_start;
						NE.SE_x = x_end;
						NE.SE_y = (y_start+y_end)/2;
						quadtree[4*j+3]=NE;	
					
						body SE;
						SE.tree_idx = 4*j+4;
						SE.array_num = -2;
						SE.NW_x = (x_start+x_end)/2;
						SE.NW_y = (y_start+y_end)/2;
						SE.SE_x = x_end;
						SE.SE_y = y_end;
						quadtree[4*j+4]=SE;
					}
					else if ((x[temp]>=(x_start+x_end)/2)&&(y[temp]>=(y_start+y_end)/2))
					{	
						//#pragma omp critical(cout)
						//cout<<temp<<" reinsert in SE"<<endl;
						// create the first node
						body NW;
						NW.tree_idx = 4*j+1;
						NW.array_num = -2;
						NW.NW_x = x_start;
						NW.NW_y = y_start;
						NW.SE_x = (x_start+x_end)/2;
						NW.SE_y = (y_start+y_end)/2;
						quadtree[4*j+1]=NW;
						// create the rest 3 nodes
						body SW;
						SW.tree_idx = 4*j+2;
						SW.array_num = -2;
						SW.NW_x = x_start;
						SW.NW_y = (y_start+y_end)/2;
						SW.SE_x = (x_start+x_end)/2;
						SW.SE_y = y_end;
						quadtree[4*j+2]=SW;
						
						body NE;
						NE.tree_idx = 4*j+3;
						NE.array_num = -2;
						NE.NW_x = (x_start+x_end)/2;
						NE.NW_y = y_start;
						NE.SE_x = x_end;
						NE.SE_y = (y_start+y_end)/2;
						quadtree[4*j+3]=NE;
						
						body SE;
						SE.tree_idx = 4*j+4;
						SE.array_num = temp;
						SE.mass_sum = m[temp];
						SE.mass_center_x = x[temp]; 
						SE.mass_center_y = y[temp];
						idx[temp] = 4*j+4;
						SE.NW_x = (x_start+x_end)/2;
						SE.NW_y = (y_start+y_end)/2;
						SE.SE_x = x_end;
						SE.SE_y = y_end;
						quadtree[4*j+4]=SE;	
					}
					// try to find a place to insert x[i]
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
					#pragma omp critical(updatemoving)
					quadtree[(j-1)/4].moving = false;	
					
				}
						
			

				else if ((quadtree[j].array_num=-3)&&(quadtree[j].moving == false))
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
			
			}
		}
		
		if (quadtree[j].array_num == -2)
		{
			body NewBody;
			NewBody.array_num = i;
			NewBody.mass_sum = m[i];
			NewBody.mass_center_x = x[i];
			NewBody.mass_center_y = y[i];
			NewBody.tree_idx = j;
			NewBody.NW_x = quadtree[j].NW_x;
			NewBody.NW_y = quadtree[j].NW_y;
			NewBody.SE_x = quadtree[j].SE_x;
			NewBody.SE_y = quadtree[j].SE_y;
			idx[i]=j;
			quadtree[j]=NewBody;
			//#pragma omp critical (cout)
			//cout<<"node "<<i<<" is inserting at quadtree "<<j<<endl;
		}
	}

	double part2_time = omp_get_wtime();
//	cout<<"quadtree size:"<<quadtree.size()<<endl;
	// for test the insertion of each nodes
//	cout<<"print out the insertion result:"<<endl;

//	for (it=quadtree.begin(); it!=quadtree.end(); it++)
//	{
		//if (it->second.array_num!=-1)
//		{
//			cout<<"cellnum["<<it->second.tree_idx<<"] = "<<it->second.array_num<<", mass_sum = "<<it->second.mass_sum
//			<<", NW = ("<<it->second.NW_x<<", "<<it->second.NW_y<<"), SE = ("<<it->second.SE_x<<", "<<it->second.SE_y<<")"<<", tree_idx = "<<it->second.tree_idx<<endl;
//		}
//	}

//	cout<<endl;
//	cout<<"the index array is:"<<endl;
//	for (int i=0; i<index; i++)
//	{
//		cout<<"index["<<i<<"]="<<idx[i]<<endl;
//	}


	
	#pragma omp parallel num_threads(thread_num)
	for (int i=0; i<5; i++)
	{
		int k=0;
		while (quadtree[0].mass_sum==-100)
		{
			// if this is a internal node
			// we need to update the mass center
			if ((quadtree[k].array_num==-3)&&(quadtree[k].mass_sum>=0))
			{
				k=0;
			}
			else if (quadtree[k].array_num>=0)
			{
				k=0;
			}
			else if (quadtree[k].array_num==-2)
			{
				k=0;
			}
			else
			{
				bool flag = false;
				#pragma omp critical(checkworking)
				if (quadtree[k].working )
				{
					k = 0;
					flag = true;
				}
				else
				{
					quadtree[k].working = true;
				}
			
				if ((quadtree[k].working)&&(!flag))
				{	
					//first we need to figure out whether the node is ready to compute 
					//the mass center
					double temp_mass_sum = 0;
					double temp_mass_center_x = 0;
					double temp_mass_center_y = 0;
			
					quadtree[k].IsReady = true;
					for (int j=1; j<5; j++)
					{
						if ((quadtree[4*k+j].array_num==-3)&&(!quadtree[4*k+j].IsReady))
						{
							quadtree[k].IsReady = false;
						}
					}
					if (quadtree[k].IsReady)
					{
						//#pragma omp critical(cout)
						for (int j=1; j<5; j++)
						{
							if (quadtree[4*k+j].mass_sum>0)
							{
								temp_mass_sum = temp_mass_sum+quadtree[4*k+j].mass_sum;
								temp_mass_center_x = temp_mass_center_x + quadtree[4*k+j].mass_sum * quadtree[4*k+j].mass_center_x;
								temp_mass_center_y = temp_mass_center_y + quadtree[4*k+j].mass_sum * quadtree[4*k+j].mass_center_y;
							}
						}
						temp_mass_center_x = temp_mass_center_x / temp_mass_sum;
						temp_mass_center_y = temp_mass_center_y / temp_mass_sum;
				
						quadtree[k].mass_sum = temp_mass_sum;
						quadtree[k].mass_center_x = temp_mass_center_x;
						quadtree[k].mass_center_y = temp_mass_center_y;
						k=0;
					}
			

					else if (!quadtree[k].IsReady)
					{
						int temp_k;
						for (int j=1; j<5; j++)
						{
							if ((quadtree[4*k+j].array_num==-3)&&(!quadtree[4*k+j].IsReady))
							{
								temp_k=4*k+j;
							}
						}
						k = temp_k;
					}
					#pragma omp critical(checkworking)
					quadtree[k].working = false;
				}
			}	

		
		
	//	if ((quadtree[k].array_num==-3)&&(quadtree[k].mass_sum<0))

		}
	}

	double part3_time = omp_get_wtime();
	//for test the computation result of mass center of each nodes
	//cout<<"after updating the mass"<<endl;
	//for (it=quadtree.begin(); it!=quadtree.end(); it++)
//	{
//		//if (it->second.array_num!=-1)
//		{
//			cout<<"cellnum["<<it->second.tree_idx<<"] = "<<it->second.array_num<<", mass_sum = "<<it->second.mass_sum
//			<<", NW = ("<<it->second.NW_x<<", "<<it->second.NW_y<<"), SE = ("<<it->second.SE_x<<", "<<it->second.SE_y<<")"<<", tree_idx = "<<it->second.tree_idx<<endl;
//		}
//	}
//	cout<<endl;

	cout<<"mass_all = "<<mass_all<<endl;
	cout<<"mass_++ = "<<quadtree[0].mass_sum<<endl;

	// approximately sort the bodies by spacial distance

	// compute forces acting on each body
	// use depth first search
	#pragma omp parallel for num_threads(thread_num)
	for (long i=0; i<index; i++)
	{
		//int TreeIdx = idx[i];
		// traverse from the root of the quadtree
		// GPU cannot use recursive
		// so we create a stack
		stack<body> works;
		works.push(quadtree[0]);

		while (works.size()!=0)
		{
			// array_num=-2 meaning that the node is a hole and have no child node
			// delete the node
			if (works.top().array_num==-2)
			{
				works.pop();
			}

			// array_num=-3 meaning that the node have child nodes
			// put all the child node into stack
			else if (works.top().array_num==-3)
			{
				double s = (works.top().SE_x - works.top().NW_x)>(works.top().SE_y - works.top().NW_y)? 
			    	           (works.top().SE_x - works.top().NW_x):(works.top().SE_y - works.top().NW_y);
			
				// distance between the current node and the mass center of the node
				double d = sqrt((x[i]-works.top().mass_center_x)*(x[i]-works.top().mass_center_x)
			 	           +(y[i]-works.top().mass_center_y)*(y[i]-works.top().mass_center_y));
				//cout<<"i = "<<i<<", d= "<<d<<endl;

	
				if (s/d<theta)
				{
					long RealIdx = idx[i];

					//compute the force
					double Forth_x = G * m[i]* works.top().mass_sum * (x[i]-works.top().mass_center_x) / (d*d*d); 	
					double Forth_y = G * m[i]* works.top().mass_sum * (y[i]-works.top().mass_center_y) / (d*d*d);
					//#pragma omp critical(cout)
					//cout<<"index = "<<RealIdx<<", Fx = "<<Forth_x<<", Fy = "<<Forth_y<<endl;
					
					quadtree[RealIdx].Fx = quadtree[RealIdx].Fx + Forth_x;
					quadtree[RealIdx].Fy = quadtree[RealIdx].Fy + Forth_y;
					fx[i] = fx[i] + Forth_x;
					fy[i] = fy[i] + Forth_y;
					works.pop();
				}
				else
				{
					long new_index = works.top().tree_idx;
					works.pop();
					works.push(quadtree[4*new_index+1]);
					works.push(quadtree[4*new_index+2]);
					works.push(quadtree[4*new_index+3]);
					works.push(quadtree[4*new_index+4]);
				}
			} 
			else if (works.top().array_num>-1)
			{
				// compute the force directly
				double d = sqrt((x[i]-works.top().mass_center_x)*(x[i]-works.top().mass_center_x)
			 	           +(y[i]-works.top().mass_center_y)*(y[i]-works.top().mass_center_y));

				long RealIdx = idx[i];
				
				if (i!=works.top().array_num)
				{

					double Forth_x = G * m[i]* works.top().mass_sum * (x[i]-works.top().mass_center_x) / (d*d*d); 	
					double Forth_y = G * m[i]* works.top().mass_sum * (y[i]-works.top().mass_center_y) / (d*d*d);
					//#pragma omp critical(cout)
					//cout<<"index = "<<index<<", Fx = "<<Forth_x<<", Fy = "<<Forth_y<<endl;

					quadtree[RealIdx].Fx = quadtree[RealIdx].Fx + Forth_x;
					quadtree[RealIdx].Fy = quadtree[RealIdx].Fy + Forth_y;
					fx[i] = fx[i] + Forth_x;
					fy[i] = fy[i] + Forth_y;
				}

				works.pop();
			}
		}
	}
	double part4_time = omp_get_wtime();
	// for test the computation result of mass center of each nodes
	cout<<"print out the result of the computation of the mass center."<<endl;
//	for (int i=0; i<TreeSize; i++)
//	{
//		if (quadtree[i].array_num!=-1)
//		{
//			cout<<"cellnum["<<i<<"] = "<<quadtree[i].array_num<<", F(x,y) = ("<<quadtree[i].Fx<<", "<<quadtree[i].Fy<<")"<<endl;
//		}
//	}
	cout<<endl;
	
	// update body position and velocities
	#pragma omp parallel for num_threads(thread_num)
	for (long i=0; i<StarNum; i++)
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

	// test the result of the final result
	for (long i=0; i<index; i++)
	{
		cout<<"x["<<i<<"]="<<x[i]<<", y["<<i<<"]="<<y[i]<<", vx["<<i<<"]="<<vx[i]<<", vy["<<i<<"]="<<vy[i]<<", fx ="<<fx[i]<<endl;
	}
	
	double end_time = omp_get_wtime();
	cout<<"elaspe time = "<<end_time-begin_time<<endl;
	cout<<"insert1 time = "<<part1_time - begin_time<<endl;
	cout<<"insert2 time = "<<part2_time - part1_time<<endl;
	cout<<"mass compute time = "<<part3_time - part2_time<<endl;
	cout<<"forth compute time = "<<part4_time - part3_time<<endl;
	return 0;

}
