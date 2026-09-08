#pragma once

#include "../../SharedIoctl/SharedIoctl.h"

class DriverLogModel
{
public:
	void add_DriverLogItem(ARR_DRIVERLOG arr);
	vecDriverLogItem get_vec_DriverLogItem();

private:
	vecDriverLogItem vec_DriverLogItem;

//typedef std::vector<DriverLogItem> vecDriverLogItem;
};

