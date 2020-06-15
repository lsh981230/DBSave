#pragma once

#pragma comment(lib, "mysqlclient.lib")
#pragma comment(lib, "winmm.lib")

#include <Windows.h>
#include <iostream>

#include <process.h>



// MYSQL 관련 헤더
#include "mysql.h"
#include "errmsg.h"

#include "CCrashDump.h"

#include "LockFreeFreeList.h"
#include "LockFreeQueue.h"
#include "MemoryPool_TLS.h"

#include "IQueryMsg.h"

#include "QMAccountRegist.h"
#include "QMPlayerInfo.h"
#include "QMStageClear.h"



#include "DBCommunicate.h"