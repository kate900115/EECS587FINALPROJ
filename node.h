#include <iostream>

using namespace std;

class node
{
	private:
		bool IsLocked;
	public:
		double ArrayNum;
		double MassCenter;
		double MassSum;
		node* NW;
		node* NE;
		node* SW;
		node* SE;
		double NW_x;
		double NW_y;
		double SE_x;
		double SE_y;
	
		node()
		{
			ArrayNum = -1; 
			MassCenter = -100; 
			MassSum= -100; 
			NW = NULL; 
			NE = NULL; 
			SW = NULL; 
			SE = NULL;
			NW_x = -1000000;
			NW_y = -1000000;
			SE_x = 1000000;
			SE_y = 1000000;
			IsLocked = false;
		}
		
		void Lock(){IsLocked = true;}
		void Unlock(){IsLocked = false;}
		
}
