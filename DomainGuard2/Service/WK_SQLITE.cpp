#include "pch.h"
#include "WK_SQLITE.h"
#include "../SQLITE/sqlite3.h"
#include "ResultQ_SQLITE.h"

WK_SQLITE::WK_SQLITE(ResultQ_SQLITE* p_ResultQ, sqlite3* p_Db, HWND handle_View)
{
	this->handle_View = handle_View;
	this->p_Db = p_Db;
	this->p_ResultQ = p_ResultQ;
	this->start_Thread();
	return;
}


void WK_SQLITE::start_Thread()
{
	AfxBeginThread(this->th_Func, this);
	return;
}

UINT AFX_CDECL WK_SQLITE::th_Func(LPVOID vp)
{
	WK_SQLITE* p_this = (WK_SQLITE*)vp;
	p_this->run();
	return 0;
}

void WK_SQLITE::run()
{
	while (1)
	{
		if (::WaitForSingleObject(this->event, INFINITE) == WAIT_OBJECT_0)
		{
			CSingleLock lock(&this->cs, FALSE);
			lock.Lock();
			auto data = q.front();
			q.pop_front();
			lock.Unlock();

			data.domain = this->paser(data.domain);

			if (data.type == sqlType::select)
			{

			}
			else if (data.type == sqlType::insert)
			{
				this->db_Insert(data.domain, data.block);
			}
			else if (data.type == sqlType::update)
			{
				this->db_Update(data.domain, data.block);
			}
			else if (data.type == sqlType::remove)
			{
				this->db_Delete(data.domain);
			}
			else if (data.type == sqlType::select_Init)
			{
				this->select_Init();
			}
			else if (data.type == sqlType::wfpInsert)
			{
				this->wfp_DB_Insert(data.wfpInfo);
			}
			else if (data.type == sqlType::wfpDelete)
			{
				this->wfp_DB_Delete(data.wfpInfo);
			}
			else if (data.type == sqlType::wfpSelect_Init)
			{
				this->wfp_DB_Select_Init();
			}
			//data.
		}
	}
	return;
}

void WK_SQLITE::wfp_DB_Select_Init()
{
	this->wfp_SelectRet.clear();

	std::string query = "SELECT * FROM wfp";
	char* errMsg = NULL;
	int ret = ::sqlite3_exec(this->p_Db, query.c_str(),
		this->wfp_selectCallBack, (void*)this, &errMsg);
	this->compare(ret, SQLITE_OK, sqlCmp::reverse, errMsg);

	Q_Finished_Data finish_Data;
	finish_Data.sql_Type = sqlType::wfpSelect_Init;
	finish_Data.vec_Info = std::move(this->wfp_SelectRet);
	this->p_ResultQ->push(finish_Data);

	::PostMessage(this->handle_View, WM_QFINISHED_FROM_WK_SQLITE, NULL, NULL);
	return;
}

int WK_SQLITE::wfp_selectCallBack(void* vp, int columnCnt,
	char** columnVal, char** columnName)
{
	WK_SQLITE* p_this = (WK_SQLITE*)vp;
	//p_this->wfp_SelectRet.clear();

	wfpFileInfo info;

	info.filePath = columnVal[0];

	std::string directionType_Cmp = columnVal[1];
	if (directionType_Cmp == "both")
	{
		info._DirectionType = DirectionType::both;
	}
	else if (directionType_Cmp == "in")
	{
		info._DirectionType = DirectionType::in;
	}
	else if (directionType_Cmp == "out")
	{
		info._DirectionType = DirectionType::out;
	}

	std::string protocolType_Cmp = columnVal[2];
	if (protocolType_Cmp == "all")
	{
		info._protocolType = protocolType::all;
	}
	else if (protocolType_Cmp == "TCP")
	{
		info._protocolType = protocolType::TCP;
	}
	else if (protocolType_Cmp == "UDP")
	{
		info._protocolType = protocolType::UDP;
	}

	std::string s_ip = columnVal[3];
	CString ss_ip;
	ss_ip.Format(_T("%S"), s_ip.c_str());
	info.ip = ss_ip;

	std::string s2_port = columnVal[4];
	CString ss2_port;
	ss2_port.Format(_T("%S"), s2_port.c_str());
	info.port = ss2_port;
	
	p_this->wfp_SelectRet.push_back(info);
	return 0;
}

void WK_SQLITE::wfp_DB_Delete(const wfpFileInfo info)
{
	//std::string query = CW2A(domain.GetString(), CP_UTF8);
	CStringA filePath_utf8 = CW2A(info.filePath, CP_UTF8);
	std::string query =
		"DELETE FROM wfp "
		"WHERE filepath = ?";

	sqlite3_stmt* stmt = NULL;
	char* errMsg = NULL;
	int ret = ::sqlite3_prepare_v2(this->p_Db, query.c_str(), -1, &stmt, NULL);
	this->compare(ret, SQLITE_OK, sqlCmp::reverse, errMsg);

	char* errMsg2 = NULL;
	int ret2 = ::sqlite3_bind_text(stmt, 1, filePath_utf8.GetString(), -1, SQLITE_TRANSIENT);
	this->compare(ret2, SQLITE_OK, sqlCmp::reverse, errMsg2);

	int ret4 = ::sqlite3_step(stmt);
	this->compare(ret4, SQLITE_DONE, sqlCmp::reverse);
	//int ret2 = ::sqlite3_exec(this->p_Db, query.c_str(), NULL, NULL, &errMsg);

	int tmp = ::sqlite3_changes(this->p_Db);

	if (ret4 == SQLITE_DONE && tmp >= 1)
	{
		Q_Finished_Data finishedData;
		finishedData.sql_Type = sqlType::wfpDelete;
		finishedData.info = info;

		this->p_ResultQ->push(finishedData);

		::PostMessage(this->handle_View, WM_QFINISHED_FROM_WK_SQLITE, NULL, NULL);
	}
	sqlite3_finalize(stmt);


	return;
	return;
	return;
}

void  WK_SQLITE::wfp_DB_Insert(const wfpFileInfo info)
{
	//std::string query = CW2A(domain.GetString(), CP_UTF8);
	CStringA filePath_utf8 = CW2A(info.filePath, CP_UTF8);
	CStringA directionType_utf8;
	CStringA protocolType_utf8;

	if (info._DirectionType == DirectionType::both)
	{
		directionType_utf8 = "both";
	}
	else if (info._DirectionType == DirectionType::in)
	{
		directionType_utf8 = "in";
	}
	else if (info._DirectionType == DirectionType::out)
	{
		directionType_utf8 = "out";
	}

	if (info._protocolType == protocolType::all)
	{
		protocolType_utf8 = "all";
	}
	else if (info._protocolType == protocolType::TCP)
	{
		protocolType_utf8 = "TCP";
	}
	else if (info._protocolType == protocolType::UDP)
	{
		protocolType_utf8 = "UDP";
	}

	CStringA ip_utf8 = CW2A(info.ip, CP_UTF8);
	CStringA port_utf8 = CW2A(info.port, CP_UTF8);

	std::string query =
		"INSERT INTO wfp "
		"(filepath, directiontype, protocoltype, ip, port) "
		"VALUES (?, ?, ?, ?, ?)";

	sqlite3_stmt* stmt = NULL;
	char* errMsg = NULL;
	int ret = ::sqlite3_prepare_v2(this->p_Db, query.c_str(), -1, &stmt, NULL);
	this->compare(ret, SQLITE_OK, sqlCmp::reverse, errMsg);

	char* errMsg2 = NULL;
	int ret2 = ::sqlite3_bind_text(stmt, 1, filePath_utf8.GetString(), -1, SQLITE_TRANSIENT);
	this->compare(ret2, SQLITE_OK, sqlCmp::reverse, errMsg2);

	char* errMsg3 = NULL;
	int ret3 = ::sqlite3_bind_text(stmt, 2, directionType_utf8.GetString(), -1, SQLITE_TRANSIENT);
	this->compare(ret3, SQLITE_OK, sqlCmp::reverse, errMsg3);

	char* errMsg4 = NULL;
	int ret4 = ::sqlite3_bind_text(stmt, 3, protocolType_utf8.GetString(), -1, SQLITE_TRANSIENT);
	this->compare(ret4, SQLITE_OK, sqlCmp::reverse, errMsg4);

	char* errMsg5 = NULL;
	int ret5 = ::sqlite3_bind_text(stmt, 4, ip_utf8.GetString(), -1, SQLITE_TRANSIENT);
	this->compare(ret5, SQLITE_OK, sqlCmp::reverse, errMsg5);

	char* errMsg6 = NULL;
	int ret6 = ::sqlite3_bind_text(stmt, 5, port_utf8.GetString(), -1, SQLITE_TRANSIENT);
	this->compare(ret6, SQLITE_OK, sqlCmp::reverse, errMsg6);

	int ret7 = ::sqlite3_step(stmt);
	this->compare(ret7, SQLITE_DONE, sqlCmp::reverse);
	//int ret2 = ::sqlite3_exec(this->p_Db, query.c_str(), NULL, NULL, &errMsg);

	int tmp = ::sqlite3_changes(this->p_Db);

	if (ret7 == SQLITE_DONE && tmp >= 1)
	{
		Q_Finished_Data finishedData;
		finishedData.sql_Type = sqlType::wfpInsert;

		finishedData.info = info;

		this->p_ResultQ->push(finishedData);

		::PostMessage(this->handle_View, WM_QFINISHED_FROM_WK_SQLITE, NULL, NULL);
	}
	sqlite3_finalize(stmt);



	return;

}

void WK_SQLITE::db_Delete(const CString domain)
{
	//std::string query = CW2A(domain.GetString(), CP_UTF8);
	CStringA domain_utf8 = CW2A(domain, CP_UTF8);
	std::string query =
		"DELETE FROM domain_guard "
		"WHERE domain = ?";

	sqlite3_stmt* stmt = NULL;
	char* errMsg = NULL;
	int ret = ::sqlite3_prepare_v2(this->p_Db, query.c_str(), -1, &stmt, NULL);
	this->compare(ret, SQLITE_OK, sqlCmp::reverse, errMsg);

	char* errMsg2 = NULL;
	int ret2 = ::sqlite3_bind_text(stmt, 1, domain_utf8.GetString(), -1, SQLITE_TRANSIENT);
	this->compare(ret2, SQLITE_OK, sqlCmp::reverse, errMsg2);

	int ret4 = ::sqlite3_step(stmt);
	this->compare(ret4, SQLITE_DONE, sqlCmp::reverse);
	//int ret2 = ::sqlite3_exec(this->p_Db, query.c_str(), NULL, NULL, &errMsg);

	int tmp = ::sqlite3_changes(this->p_Db);
	if (ret4 == SQLITE_DONE && tmp >= 1)
	{
		Q_Finished_Data finishedData;
		finishedData.sql_Type = sqlType::remove;
		blockDomain _blockDomain;
		_blockDomain.domain = domain;

		finishedData._blockDomain = _blockDomain;

		this->p_ResultQ->push(finishedData);

		::PostMessage(this->handle_View, WM_QFINISHED_FROM_WK_SQLITE, NULL, NULL);
	}
	sqlite3_finalize(stmt);


	return;
	return;
}

void WK_SQLITE::select_Init()
{
	this->selectRet.clear();

	std::string query = "SELECT * FROM domain_guard";
	char* errMsg = NULL;
	int ret = ::sqlite3_exec(this->p_Db, query.c_str(),
		this->selectCallBack, (void*)this, &errMsg);
	this->compare(ret, SQLITE_OK, sqlCmp::reverse, errMsg);

	Q_Finished_Data finish_Data;
	finish_Data.sql_Type = sqlType::select_Init;
	finish_Data.vec_BlockDomain = std::move(this->selectRet);
	this->p_ResultQ->push(finish_Data);

	::PostMessage(this->handle_View, WM_QFINISHED_FROM_WK_SQLITE, NULL, NULL);
	return;
}

int WK_SQLITE::selectCallBack(void* vp, int columnCnt,
	char** columnVal, char** columnName)
{
	WK_SQLITE* p_this = (WK_SQLITE*)vp;

	blockDomain bkDomain = { 0 };
	bkDomain.domain = columnVal[0];

	if (columnVal[1][0] == '0')
	{
		bkDomain.block = false;
	}
	else if (columnVal[1][0] == '1')
	{
		bkDomain.block = true;
	}

	p_this->selectRet.push_back(bkDomain);
	return 0;
}

void WK_SQLITE::db_Insert(const CString domain, const bool block)
{
	//std::string query = CW2A(domain.GetString(), CP_UTF8);
	CStringA domain_utf8 = CW2A(domain, CP_UTF8);
	std::string query =
		"INSERT INTO domain_guard "
		"(domain, block) "
		"VALUES (?, ?)";

	sqlite3_stmt* stmt = NULL;
	char* errMsg = NULL;
	int ret = ::sqlite3_prepare_v2(this->p_Db, query.c_str(), -1, &stmt, NULL);
	this->compare(ret, SQLITE_OK, sqlCmp::reverse, errMsg);

	char* errMsg2 = NULL;
	int ret2 = ::sqlite3_bind_text(stmt, 1, domain_utf8.GetString(), -1, SQLITE_TRANSIENT);
	this->compare(ret2, SQLITE_OK, sqlCmp::reverse, errMsg2);

	char* errMsg3 = NULL;
	int ret3 = ::sqlite3_bind_int(stmt, 2, block);
	this->compare(ret3, SQLITE_OK, sqlCmp::reverse, errMsg3);

	int ret4 = ::sqlite3_step(stmt);
	this->compare(ret4, SQLITE_DONE, sqlCmp::reverse);
	//int ret2 = ::sqlite3_exec(this->p_Db, query.c_str(), NULL, NULL, &errMsg);

	int tmp = ::sqlite3_changes(this->p_Db);
	if (ret4 == SQLITE_DONE && tmp >= 1)
	{
		Q_Finished_Data finishedData;
		finishedData.sql_Type = sqlType::insert;
		blockDomain _blockDomain;
		_blockDomain.block = block;
		_blockDomain.domain = domain;

		finishedData._blockDomain = _blockDomain;

		this->p_ResultQ->push(finishedData);

		::PostMessage(this->handle_View, WM_QFINISHED_FROM_WK_SQLITE, NULL, NULL);
	}
	sqlite3_finalize(stmt);


	return;
}

bool WK_SQLITE::compare(int val1, int val2, sqlCmp type, char* errMsg)
{
	if (type == sqlCmp::reverse)
	{
		if (val1 != val2)
		{
			const char* msg = ::sqlite3_errmsg(this->p_Db);
			CString tmp = CA2W(msg, CP_UTF8);

			CString err;
			err.Format(_T("\nERR ___ %S\n%s\n"), errMsg, tmp);
			OutputDebugString(err);
		}
	}
	return true;
}
CString  WK_SQLITE::paser(CString s)
{
	return s;
}

void WK_SQLITE::Q_Push(sql_Q_Job job)
{
	CSingleLock
		lock(&this->cs, TRUE);
	this->q.push_back(job);

	this->event.SetEvent();
}

void WK_SQLITE::db_Update(const CString domain, const bool block)
{
	//std::string query = CW2A(domain.GetString(), CP_UTF8);
	CStringA domain_utf8 = CW2A(domain, CP_UTF8);
	std::string query =
		"UPDATE domain_guard "
		"SET block = ? "
		"WHERE domain = ?";

	sqlite3_stmt* stmt = NULL;
	char* errMsg = NULL;
	int ret = ::sqlite3_prepare_v2(this->p_Db, query.c_str(), -1, &stmt, NULL);
	this->compare(ret, SQLITE_OK, sqlCmp::reverse, errMsg);

	char* errMsg2 = NULL;
	int ret2 = ::sqlite3_bind_int(stmt, 1, block);
	this->compare(ret2, SQLITE_OK, sqlCmp::reverse, errMsg2);

	char* errMsg3 = NULL;
	int ret3 = ::sqlite3_bind_text(stmt, 2, domain_utf8.GetString(), -1, SQLITE_TRANSIENT);
	this->compare(ret3, SQLITE_OK, sqlCmp::reverse, errMsg3);

	int ret4 = ::sqlite3_step(stmt);
	this->compare(ret4, SQLITE_DONE, sqlCmp::reverse);
	//int ret2 = ::sqlite3_exec(this->p_Db, query.c_str(), NULL, NULL, &errMsg);

	int tmp = ::sqlite3_changes(this->p_Db);
	if (ret4 == SQLITE_DONE && tmp >= 1)
	{
		Q_Finished_Data finishedData;
		finishedData.sql_Type = sqlType::update;
		blockDomain _blockDomain;
		_blockDomain.block = block;
		_blockDomain.domain = domain;

		finishedData._blockDomain = _blockDomain;

		this->p_ResultQ->push(finishedData);

		::PostMessage(this->handle_View, WM_QFINISHED_FROM_WK_SQLITE, NULL, NULL);
	}


	sqlite3_finalize(stmt);

	return;
}




