#pragma once

#pragma comment(lib, "mysqlclient.lib")

#include <Windows.h>
#include <iostream>

#include <process.h>



// MYSQL ���� ���
#include "mysql.h"
#include "errmsg.h"


#include "LockFreeFreeList.h"
#include "LockFreeQueue.h"

#include "IQueryMsg.h"

#include "QMAccountRegist.h"
#include "QMPlayerInfo.h"
#include "QMStageClear.h"



#include "DBCommunicate.h"