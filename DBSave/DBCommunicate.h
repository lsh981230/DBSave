#pragma once

class DBCommunicate
{
public:
	DBCommunicate();
	~DBCommunicate();

	void	DisconnectDB();

private:
	static UINT WINAPI DBSaveThread(LPVOID);
	static UINT WINAPI UpdateThread(LPVOID);


	bool	DBConnect();
	void	CreateThread();

	void	EnqueueMsg(IQueryMsg* pMsg);
	void	SwitchMsg();
	void	SendQuery(char* pQuery);

	bool ConnectError(int errorNo);


public:

	LockFreeQueue<IQueryMsg*>*	_msgQueue;

	int		_queryTPS;
	MYSQL*	_dbLink;

	HANDLE	_hDBThread;
	HANDLE	_hMsgEnQ;

	bool	_bStopDB;
};

