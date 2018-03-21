/*Example based on http://www.cplusplus.com/reference/thread/thread/ */

#include "ConcurrencyEx.h"

void func1(int x)
{
	cout << "func1 " << x << "\n";
}

void func2(int x)
{
	cout << "func2 " << x << "\n";
}


int ThreadEx(void)
{
	cout << "func1 and func2 will execute concurrently\n";

	std::thread first(func1,123);
	std::thread second(func2,314);

	

	first.join();//pauses until first finishes
	second.join();//pauses until second finishes

	return 0;
}