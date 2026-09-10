#pragma once

class WK_DNS;
class DomainBlockModel;
class WK_DNS_GQCS;
class ProxyID_Store;

class DNS
{
public:
	DNS(DomainBlockModel* p_DomainBlockModel, HWND h_View);
	~DNS();
	void create_WK();

	un_map_DNSLogItem get_un_map_DNSLogItem();

	void close();
	void delete_p_WK__p_WK_GQCS();

private:
	SOCKET soc = NULL;
	WSADATA wsaData = {};

	SOCKET wsaSoc= NULL;
	HANDLE h_Iocp = NULL;

	WK_DNS* p_WK = nullptr;

	WK_DNS_GQCS* p_WK_GQCS = nullptr;

	DomainBlockModel* p_DomainBlockModel = nullptr;
	ProxyID_Store* p_ProxyID_Store = nullptr;

	HWND h_View = nullptr;
};

