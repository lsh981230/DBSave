#include "stdafx.h"

QMAccountRegist::QMAccountRegist()
{
}

QMAccountRegist::QMAccountRegist(char * schemaName, char * tableName)
{
	_value = 0;
	_schemaName = schemaName;
	_tableName = tableName;
}


QMAccountRegist::~QMAccountRegist()
{
}

char * QMAccountRegist::CreateQuery()
{
	ZeroMemory(_query, sizeof(_query));

	sprintf_s(_query, "INSERT into %s.%s (value) VALUES (%d)", _schemaName, _tableName, ++_value);


	return _query;
}
