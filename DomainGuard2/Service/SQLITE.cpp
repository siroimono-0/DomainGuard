#include "pch.h"
#include "SQLITE.h"
#include "../SQLITE/sqlite3.h"
#include <string>
#include "WK_SQLITE.h"
#include "ResultQ_SQLITE.h"

SQLITE::SQLITE(ResultQ_SQLITE* p_ResultQ, HWND handle_View)
{
	this->handle_View = handle_View;
	this->p_ResultQ = p_ResultQ;

	int ret = sqlite3_open_v2("sqlite.db", &this->p_Db,
		SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);

	if (ret != SQLITE_OK)
	{
		std::string s = sqlite3_errmsg(this->p_Db);
		CString cs;
		cs.Format(_T("%s"), s);

		OutputDebugString(cs);
	}

	std::string query;
	query = "CREATE TABLE ""domain_guard"" ("
		"domain TEXT PRIMARY KEY,"
		"block  BLOB);";

	char* errMsg = NULL;
	int ret2 = sqlite3_exec(this->p_Db, query.c_str(), NULL, NULL, &errMsg);
	if (ret2 != SQLITE_OK)
	{
		std::string s1 = errMsg;
		CString s2;
		s2.Format(_T("%S"), s1.c_str());
		OutputDebugString(s2);
	}

	std::string query2;
	query2 =
		"CREATE TABLE ""wfp"" ("
		"filepath      TEXT    PRIMARY KEY,"
		"directiontype TEXT,"
		"protocoltype  TEXT,"
		"ip            TEXT,"
		"port          TEXT"
		");";


	char* errMsg2 = NULL;
	int ret3 = sqlite3_exec(this->p_Db, query2.c_str(), NULL, NULL, &errMsg2);
	if (ret3 != SQLITE_OK)
	{
		std::string s3 = errMsg2;
		CString s4;
		s4.Format(_T("%S"), s3.c_str());
		OutputDebugString(s4);
	}

	this->create_WK();

	sql_Q_Job initJob;
	initJob.type = sqlType::select_Init;
	this->push_WK_Q(initJob);

	sql_Q_Job wfp_initJob;
	wfp_initJob.type = sqlType::wfpSelect_Init;
	this->push_WK_Q(wfp_initJob);

	return;
}

void SQLITE::create_WK()
{
	this->p_wk = new WK_SQLITE(this->p_ResultQ, this->p_Db, this->handle_View);
	return;
}

void SQLITE::push_WK_Q(sql_Q_Job job)
{
	this->p_wk->Q_Push(job);
}

Q_Finished_Data SQLITE::tryPop_WK_Q()
{
	Q_Finished_Data data;
	if (this->p_ResultQ->tryPop(data))
	{
		OutputDebugString(_T("Q Empty ___ tryPop_WK_Q"));
	}
	return std::move(data);
}

