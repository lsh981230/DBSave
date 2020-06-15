#pragma once
class QMAccountRegist :
	public IQueryMsg
{
public:
	QMAccountRegist();
	QMAccountRegist(char* schemaName, char* tableName, int value);
	virtual ~QMAccountRegist();

	char* CreateQuery() override;

public:

	int		_value;
	char	_ID[20];
	char	_password[20];
};

