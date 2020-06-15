#pragma once
#include <Windows.h>
class IQueryMsg
{
public:
	IQueryMsg() {}
	virtual ~IQueryMsg() {}

	virtual char* CreateQuery() = 0;

protected:

	char	_query[256];

	char	_schemaName[32];
	char	_tableName[32];
};

