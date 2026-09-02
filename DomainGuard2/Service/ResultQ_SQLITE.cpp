#include "pch.h"
#include "ResultQ_SQLITE.h"

void ResultQ_SQLITE::push(Q_Finished_Data data)
{
	CSingleLock lock(&this->cs, TRUE);
	q.push_back(data);
	return;
}

bool ResultQ_SQLITE::tryPop(Q_Finished_Data& data)
{
	CSingleLock lock(&this->cs, TRUE);
	if (this->q.empty())
	{
		return false;
	}


	data = q.front();
	q.pop_front();

	return true;
}
