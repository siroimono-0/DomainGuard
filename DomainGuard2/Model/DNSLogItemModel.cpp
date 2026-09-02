#include "pch.h"
#include "DNSLogItemModel.h"

DNSLogItemModel::DNSLogItemModel(HWND handle_View)
{
	this->handle_View = handle_View;
	return;
}

void DNSLogItemModel::update_vec_DNSLogItem(un_map_DNSLogItem item)
{
	for (auto it = item.begin(); it != item.end(); it++)
	{
		this->vec_DNSLogItem.push_back(it->second);
	}
	this->cur_UpdateData = item;
	return;
}

un_map_DNSLogItem DNSLogItemModel::get_un_map_DNSLogItem()
{
	return std::move(this->cur_UpdateData);
}
