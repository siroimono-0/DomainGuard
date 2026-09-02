#pragma once

#include "pch.h"
#include <queue>
struct sql_Q_Job;

class ResultQ_SQLITE
{

public:
	void push(Q_Finished_Data data);
	bool tryPop(Q_Finished_Data& data);

private:
	CEvent event;
	CCriticalSection cs;

	std::deque<Q_Finished_Data> q;
};

