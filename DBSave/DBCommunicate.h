#pragma once

class DBCommunicate
{
public:
	DBCommunicate();
	~DBCommunicate();

	void	DisconnectDB();
	void	Print();


private:
	static UINT WINAPI DBSaveThread(LPVOID);
	static UINT WINAPI UpdateThread(LPVOID);


	bool	DBConnect();
	void	CreateThread();

	void	EnqueueMsg(IQueryMsg* pMsg);
	void	SwitchMsg();
	void	SendQuery(char* pQuery);

	bool	ConnectError(int errorNo);

	void	DBTruncate();


public:

	int		_queryTPS;
	MYSQL	_conn;
	MYSQL*	_dbLink;

	HANDLE	_hDBThread;
	HANDLE	_hMsgEnQ;

	bool	_bStopDB;
	LockFreeQueue<IQueryMsg*>*	_msgQueue;
};

