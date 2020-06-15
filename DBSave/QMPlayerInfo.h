#pragma once
class QMPlayerInfo :	public IQueryMsg
{
public:
	QMPlayerInfo();
	QMPlayerInfo(char* schemaName, char* tableName, int value);
	virtual ~QMPlayerInfo();


	char*	CreateQuery() override;

public:

	int		_value;
	int		_level;
	__int64	_accountNo;
	__int64 _exp;

};

