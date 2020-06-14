
#include "stdafx.h"
#include "DBCommunicate.h"



DBCommunicate::DBCommunicate()
{
	_bStopDB = false;
	_msgQueue = new LockFreeQueue<IQueryMsg*>();
	_hMsgEnQ = CreateEvent(nullptr, true, false, nullptr);

	//CreateThread();

	if (!DBConnect())
		exit(-1);

	_hDBThread = (HANDLE)_beginthreadex(nullptr, 0, DBSaveThread, this, 0, nullptr);
	HANDLE hUpdateThread = (HANDLE)_beginthreadex(nullptr, 0, UpdateThread, this, 0, nullptr);
	CloseHandle(hUpdateThread);

}


DBCommunicate::~DBCommunicate()
{
}


void DBCommunicate::DisconnectDB()
{
	_bStopDB = true;


	// DB Thread�� ������� ���
	WaitForSingleObject(_hDBThread, INFINITE);
	CloseHandle(_hDBThread);
}




void DBCommunicate::Print()
{
	printf("=============================================\n");
	printf("Query TPS : %d\n\n", _queryTPS);
	printf("=============================================\n");

	_queryTPS = 0;
}




bool DBCommunicate::DBConnect()
{
	MYSQL conn;


	// �ʱ�ȭ
	mysql_init(&conn);


	// DB ����
	_dbLink = mysql_real_connect(&conn, "127.0.0.1", "root", "root", "test_server", 3306, (char *)NULL, 0);
	if (_dbLink == NULL)
	{
		fprintf(stderr, "ERROR : %s", mysql_error(&conn));
		return false;
	}



	return true;
}



UINT __stdcall DBCommunicate::DBSaveThread(LPVOID arg)
{
	//DBCommunicate* pThis = (DBCommunicate*)arg;



	//for (;;)
	//{
	//	WaitForSingleObject(pThis->_hMsgEnQ, INFINITE);



	//	while (pThis->_msgQueue->GetQueueSize() > 0)
	//	{

	//		// Message ó��			
	//		pThis->SwitchMsg();
	//	}



	//	// ���� ����
	//	if (pThis->_msgQueue->GetQueueSize() <= 0 && pThis->_bStopDB)
	//		break;
	//}



	//// DB�� ��������
	//mysql_close(pThis->_dbLink);

	return 0;
}



UINT __stdcall DBCommunicate::UpdateThread(LPVOID arg)
{
	//DBCommunicate* pThis = (DBCommunicate*)arg;
	//HANDLE hNeverSignalEvent = CreateEvent(nullptr, true, false, nullptr);



	//while (!pThis->_bStopDB)
	//{
	//	//--------------------------------------------------------
	//	// 1. Alloc Data Block 
	//	//--------------------------------------------------------

	//	void* pData = malloc(sizeof(QMAccountRegist));		
	//	ZeroMemory(pData, sizeof(QMAccountRegist));



	//	//--------------------------------------------------------
	//	// 2. placement new
	//	//--------------------------------------------------------

	//	switch (rand() % 3)
	//	{
	//	case 0:
	//	{
	//		pData = new (pData)QMAccountRegist((char*)"test_server", (char*)"account");
	//		pThis->EnqueueMsg((QMAccountRegist*)pData);
	//		break;
	//	}

	//	case 1:
	//	{
	//		pData = new (pData)QMPlayerInfo((char*)"test_server", (char*)"player");
	//		pThis->EnqueueMsg((QMPlayerInfo*)pData);
	//		break;
	//	}

	//	case 2:
	//	{
	//		pData = new (pData)QMStageClear((char*)"test_server", (char*)"stage");
	//		pThis->EnqueueMsg((QMStageClear*)pData);
	//		break;
	//	}
	//	}



	//	//--------------------------------------------------------
	//	// 3. Sleep
	//	//--------------------------------------------------------

	//	WaitForSingleObject(hNeverSignalEvent, 10000000);

	//}


	return 0;
}





void DBCommunicate::CreateThread()
{
	_hDBThread = (HANDLE)_beginthreadex(nullptr, 0, DBSaveThread, this, 0, nullptr);

	HANDLE hUpdateThread = (HANDLE)_beginthreadex(nullptr, 0, UpdateThread, this, 0, nullptr);
	CloseHandle(hUpdateThread);
}




void DBCommunicate::EnqueueMsg(IQueryMsg * pMsg)
{
	_msgQueue->Enqueue(pMsg);
	SetEvent(_hMsgEnQ);
}




void DBCommunicate::SwitchMsg()
{

	//--------------------------------------------------------
	// 1. MsgQueue���� Dequeue
	//--------------------------------------------------------

	IQueryMsg* pMsg = nullptr;
	if (!_msgQueue->Dequeue(&pMsg))
		exit(0);



	//--------------------------------------------------------
	// 2. Query ����
	//--------------------------------------------------------

	char* pQuery = pMsg->CreateQuery();



	//--------------------------------------------------------
	// 3. Query ����
	//--------------------------------------------------------

	SendQuery(pQuery);

}




void DBCommunicate::SendQuery(char* pQuery)
{
	//int queryRes = mysql_query(_dbLink, "SELECT * FROM test_server.account");
	int queryRes = mysql_query(_dbLink, pQuery);

	if (queryRes != 0)
	{
		if (ConnectError(mysql_errno(_dbLink)))
		{
			mysql_query(_dbLink, pQuery);
		}

		else
		{
			exit(0);
		}

	}
}



bool DBCommunicate::ConnectError(int errorNo)
{
	fprintf(stderr, "ERROR : %s", mysql_error(_dbLink));

	// Try Reconnect
	if (errorNo == CR_SOCKET_CREATE_ERROR
		|| errorNo == CR_CONNECTION_ERROR
		|| errorNo == CR_CONN_HOST_ERROR
		|| errorNo == CR_SERVER_GONE_ERROR
		|| errorNo == CR_SERVER_HANDSHAKE_ERR
		|| errorNo == CR_SERVER_LOST
		|| errorNo == CR_INVALID_CONN_HANDLE)
	{

		for (int i = 0; i < 20; i++)
		{
			if (mysql_ping(_dbLink) == 0)
			{
				return true;
			}
		}


	}

	return false;
}

