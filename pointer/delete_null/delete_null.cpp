#include <iostream>
using namespace std;

int main()
{
	cout<<"delete null pointer test"<<endl;
	
	int *p = NULL;
	if(p == NULL){
		cout<<"delete"<<endl;
		delete p;
	}

	cout<<"done"<<endl;
	return 0;
}