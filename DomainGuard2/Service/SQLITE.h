#pragma once
#include "../SQLITE/sqlite3.h"

class WK_SQLITE;
class ResultQ_SQLITE;
struct sql_Q_Job;

class SQLITE
{

public:
	SQLITE(ResultQ_SQLITE* p_ResultQ, HWND handle_View);

	void create_WK();

	void push_WK_Q(sql_Q_Job job);
	//void set_handle_View(HWND handle);
	Q_Finished_Data tryPop_WK_Q();

	void delete_WK();
private:
	sqlite3* p_Db = nullptr;
	WK_SQLITE* p_wk = nullptr;
	ResultQ_SQLITE* p_ResultQ = nullptr;
	HWND handle_View = NULL;
};

