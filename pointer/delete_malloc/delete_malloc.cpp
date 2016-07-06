#include <iostream>
#include <malloc.h>
using namespace std;

int main()
{
	cout<<"delete malloc test"<<endl;
	
	char *p = (char *)malloc(10);
	sprintf(p, "%s", "0123456789");

	cout<<&p<<endl;
	delete p;
	//free(p);
	cout<<&p<<endl;
	
	return 0;
}
