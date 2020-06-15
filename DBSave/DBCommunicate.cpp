
#include "stdafx.h"
#include "DBCommunicate.h"



DBCommunicate::DBCommunicate()
{
	_bStopDB = false;
	_msgQueue = new LockFreeQueue<IQueryMsg*>();
	_memoryPool = new MemoryPool_TLS<QMAccountRegist>(true, TlsAlloc());
	_hMsgEnQ = CreateEvent(nullptr, true, false, nullptr);

	if (!DBConnect())
		exit(-1);

	DBTruncate();

	CreateThread();

}


DBCommunicate::~DBCommunicate()
{
}


void DBCommunicate::DisconnectDB()
{
	_bStopDB = true;


	// DB Thread의 종료까지 대기
	WaitForSingleObject(_hDBThread, INFINITE);
	CloseHandle(_hDBThread);
}




void DBCommunicate::Print()
{
	printf("=============================================\n");
	printf("Query TPS : %d\n", _queryTPS);
	printf("Queue Size : %d\n\n", _msgQueue->GetQueueSize());

	printf("MemoryPool Use Count : %d\n", _memoryPool->GetUseCount());
	printf("MemoryPool Alloc Count : %d\n", _memoryPool->GetAllocCnt());
	printf("=============================================\n");

	_queryTPS = 0;
}




bool DBCommunicate::DBConnect()
{
	// 초기화
	mysql_init(&_conn);


	// DB 연결
	_dbLink = mysql_real_connect(&_conn, "127.0.0.1", "root", "root", "test_server", 3306, (char *)NULL, 0);
	if (_dbLink == NULL)
	{
		fprintf(stderr, "ERROR : %s", mysql_error(&_conn));
		return false;
	}

	


	return true;
}



UINT __stdcall DBCommunicate::DBSaveThread(LPVOID arg)
{
	DBCommunicate* pThis = (DBCommunicate*)arg;



	for (;;)
	{
		WaitForSingleObject(pThis->_hMsgEnQ, INFINITE);



		while (pThis->_msgQueue->GetQueueSize() > 0)
		{

			// Message 처리			
			pThis->SwitchMsg();
		}



		// 종료 조건
		if (pThis->_bStopDB && pThis->_msgQueue->GetQueueSize() <= 0)
			break;
	}



	// DB와 연결종료
	mysql_close(pThis->_dbLink);

	return 0;
}



UINT __stdcall DBCommunicate::UpdateThread(LPVOID arg)
{
	DBCommunicate* pThis = (DBCommunicate*)arg;
	HANDLE hNeverSignalEvent = CreateEvent(nullptr, true, false, nullptr);

	DWORD accountCnt = 0;
	DWORD playerCnt = 0;
	DWORD stageCnt = 0;

	while (!pThis->_bStopDB)
	{

		//--------------------------------------------------------
		// 1. Alloc Data Block 
		//--------------------------------------------------------

		void* pData = pThis->_memoryPool->Alloc();
		ZeroMemory(pData, sizeof(QMAccountRegist));



		//--------------------------------------------------------
		// 2. placement new
		//--------------------------------------------------------


		switch (rand() % 3)
		{
		case 0:
		{
			pData = new (pData)QMAccountRegist((char*)"test_server", (char*)"account", accountCnt++);
			pThis->EnqueueMsg((QMAccountRegist*)pData);
			break;
		}

		case 1:
		{
			pData = new (pData)QMPlayerInfo((char*)"test_server", (char*)"player", playerCnt++);
			pThis->EnqueueMsg((QMPlayerInfo*)pData);
			break;
		}

		case 2:
		{
			pData = new (pData)QMStageClear((char*)"test_server", (char*)"stage", stageCnt++);
			pThis->EnqueueMsg((QMStageClear*)pData);
			break;
		}
		}


		//--------------------------------------------------------
		// 3. Sleep
		//--------------------------------------------------------

		WaitForSingleObject(hNeverSignalEvent, 2);

	}


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
	// 1. MsgQueue에서 Dequeue
	//--------------------------------------------------------

	IQueryMsg* pMsg = nullptr;
	if (!_msgQueue->Dequeue(&pMsg))
		exit(0);



	//--------------------------------------------------------
	// 2. Query 생성
	//--------------------------------------------------------

	char* pQuery = pMsg->CreateQuery();



	//--------------------------------------------------------
	// 3. Query 전송
	//--------------------------------------------------------

	SendQuery(pQuery);



	//--------------------------------------------------------
	// 4. Message Free
	//--------------------------------------------------------
	
	_memoryPool->Free((QMAccountRegist*)pMsg);

}




void DBCommunicate::SendQuery(char* pQuery)
{
	int queryRes = mysql_query(_dbLink, pQuery);

	if (queryRes != 0)
	{
		printf("Query Failed, Query was : %s\n", pQuery);

		if (ConnectError(mysql_errno(_dbLink)))
		{
			mysql_query(_dbLink, pQuery);
		}

		else
		{
			exit(0);
		}

	}
	else
		_queryTPS++;
}



bool DBCommunicate::ConnectError(int errorNo)
{
	fprintf(stderr, "Query Error : %s\n", mysql_error(_dbLink));

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
			if (DBConnect())
			{
				printf("Reconnect Success\n");
				return true;
			}
		}


	}

	printf("Reconnect Failed\n");
	return false;
}


void DBCommunicate::DBTruncate()
{
	SendQuery((char*)"truncate test_server.account;");
	SendQuery((char*)"truncate test_server.player;");
	SendQuery((char*)"truncate test_server.stage;");
}

