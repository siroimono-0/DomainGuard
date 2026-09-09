#pragma once

#include "../../SharedIoctl/SharedIoctl.h"

class DriverLogModel
{
public:
	void add_DriverLogItem(ARR_DRIVERLOG arr);
	vecDriverLogItem get_tmp_vec_DriverLogItem();

private:
	vecDriverLogItem vec_DriverLogItem;
	vecDriverLogItem tmp_vec_DriverLogItem;

//typedef std::vector<DriverLogItem> vecDriverLogItem;
};

