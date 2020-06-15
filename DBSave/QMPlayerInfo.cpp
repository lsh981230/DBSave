#include "stdafx.h"


QMPlayerInfo::QMPlayerInfo()
{
}

QMPlayerInfo::QMPlayerInfo(char * schemaName, char * tableName, int value)
{
	_value = value;
	strcpy_s(_schemaName, schemaName);
	strcpy_s(_tableName, tableName);
}


QMPlayerInfo::~QMPlayerInfo()
{
}

char * QMPlayerInfo::CreateQuery()
{
	ZeroMemory(_query, sizeof(_query));


	sprintf_s(_query, "INSERT into %s.%s (value) VALUES (%d)", _schemaName, _tableName, _value);


	return _query;
}
