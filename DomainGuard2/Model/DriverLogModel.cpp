#include "pch.h"
#include "DriverLogModel.h"

void  DriverLogModel::add_DriverLogItem(DriverLogItem item)
{
	this->vec_DriverLogItem.push_back(item);
	return;
}

vecDriverLogItem DriverLogModel::get_vec_DriverLogItem()
{
	return this->vec_DriverLogItem;
}
