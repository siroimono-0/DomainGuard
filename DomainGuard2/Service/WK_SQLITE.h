#pragma once
#include "pch.h"
#include <queue>
#include "../SQLITE/sqlite3.h"

struct sql_Q_Job;
class ResultQ_SQLITE;

class WK_SQLITE
{
public:
	WK_SQLITE(ResultQ_SQLITE* p_ResultQ, sqlite3* p_Db, HWND handle_View);
	void start_Thread();
	static UINT AFX_CDECL th_Func(LPVOID vp);

	void Q_Push(sql_Q_Job job);
	void run();

	CString paser(CString s);

	void db_Insert(const CString domain, const bool block);
	void db_Update(const CString domain, const bool block);
	void db_Delete(const CString domain);

	bool compare(int val1, int val2, sqlCmp type, char* errMsg = NULL);
	void select_Init();
	static int selectCallBack(void* vp, int columnCnt,
		char** columnVal, char** columnName);

	void wfp_DB_Insert(const wfpFileInfo info);
	void wfp_DB_Delete(const wfpFileInfo info);
	void wfp_DB_Select_Init();
	static int wfp_selectCallBack(void* vp, int columnCnt,
		char** columnVal, char** columnName);

private:
	std::deque<sql_Q_Job> q;
	CEvent event;
	CCriticalSection cs;
	ResultQ_SQLITE* p_ResultQ;
	sqlite3* p_Db = nullptr;

	HWND handle_View = NULL;

	std::vector<blockDomain> selectRet;
	vecWfpFileInfo wfp_SelectRet;
	//std::vector<wfpFileInfo> 
};

