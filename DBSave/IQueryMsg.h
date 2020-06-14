#pragma once
#include <Windows.h>
class IQueryMsg
{
public:
	IQueryMsg() {}
	virtual ~IQueryMsg() {}

	virtual char* CreateQuery() = 0;

protected:

	char	_query[128];

	char*	_schemaName;
	char*	_tableName;
};

