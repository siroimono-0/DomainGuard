#pragma once
class DNSLogItemModel
{
public:
	DNSLogItemModel(HWND handle_View);

	void update_vec_DNSLogItem(un_map_DNSLogItem item);

	un_map_DNSLogItem get_un_map_DNSLogItem();
private:
	HWND handle_View = NULL;
	vector<DNSLogItem> vec_DNSLogItem;
	un_map_DNSLogItem cur_UpdateData;
};

