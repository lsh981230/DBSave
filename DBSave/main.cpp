
#include "stdafx.h"


int main()
{
	timeBeginPeriod(1);

	DBCommunicate* db = new DBCommunicate();

	for (;;)
	{
		db->Print();
		Sleep(1000);
	}

}
