#pragma once

class ProxyID_Store;

class WK_DNS_GQCS
{
public:
	WK_DNS_GQCS(SOCKET& soc, SOCKET& wsaSoc,
		HANDLE h_Iocp, ProxyID_Store* p_ProxyID_Store, 
		HWND h_View);
	void start_Thread();
	static UINT AFX_CDECL th_Func(LPVOID vp);
	void run();

	uint16_t get_ProxtID(uint8_t* packet);
	void cnv_ProxyID_To_orgID(uint8_t* packet, uint16_t orgID);

	void update_Allowed_DNSLogItem(uint16_t proxyID);

private:
	SOCKET soc = NULL;
	SOCKET wsaSoc = NULL;
	HANDLE h_Iocp = NULL;
	ProxyID_Store* p_ProxyID_Store = nullptr;

	HWND h_View = nullptr;
};

