#pragma once
class QMStageClear :
	public IQueryMsg
{
public:
	QMStageClear();
	QMStageClear(char* schemaName, char* tableName);

	~QMStageClear();

	char* CreateQuery() override;

public:

	int		_value;
	int		_stageID;
	__int64	_accountNo;
};

