#include "IOCP/IOCP.h"

int main()
{
	if (GET_INSTANCE(IOCP)->Initialize() == false)
	{
		cout << "IOCP Server Initialize Fail!!" << endl;
		return -1;
	}

	GET_INSTANCE(IOCP)->Run();
	GET_INSTANCE(IOCP)->Release();
}
