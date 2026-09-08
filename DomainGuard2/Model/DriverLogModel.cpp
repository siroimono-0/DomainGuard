#include "pch.h"
#include "DriverLogModel.h"

void  DriverLogModel::add_DriverLogItem(ARR_DRIVERLOG arr)
{
	for (int i = 0; i < 32; i++)
	{
		if (arr.arrDriverLog[i].domain[0] != 0)
		{
			DriverLogItem item;
			//item.
		}
	}
	//this->vec_DriverLogItem.push_back(item);
	return;
}

vecDriverLogItem DriverLogModel::get_vec_DriverLogItem()
{
	return this->vec_DriverLogItem;
}
