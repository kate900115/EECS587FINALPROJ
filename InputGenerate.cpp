#include <iostream>
#include <ctime> 
#include <cstdlib> 

using namespace std;

int main()
{
	srand(time(NULL)); 

	// generate 5 random numbers 
	for(int i=0; i<500000; i++)
	{ 
		if ((i!=0)&&(i%5==0))
		{
			cout<<endl<<(rand()%100000)<<" ";
		}
		else
		{
			cout<<(rand()%100000)<<" ";
		}
	} 
return 0; 
}
