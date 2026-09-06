#pragma once
class DriverLogModel
{
public:
	void add_DriverLogItem(DriverLogItem item);
	vecDriverLogItem get_vec_DriverLogItem();
private:
	vecDriverLogItem vec_DriverLogItem;

//typedef std::vector<DriverLogItem> vecDriverLogItem;
};

