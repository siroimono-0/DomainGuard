#pragma once
#include <string>

class DomainBlockModel;
class ProxyID_Store;

class WK_DNS
{

public:
	WK_DNS(SOCKET& soc, SOCKET& wsaSoc,
		sockaddr_in addr_Sendto, HANDLE h_iocp, DomainBlockModel* p_Model,
		ProxyID_Store* p_ProxyID_Store, HWND h_View);
	void start_Thread();
	static UINT AFX_CDECL th_Func(LPVOID vp);
	void run();

	std::string paser(uint8_t* packet, int packetLen, int& questionEndOffset);

	void sendto_Blocked(const uint8_t* packet, int packetLen, const sockaddr_in clientAddr,
		int questionEndOffset);
	void sendto_Nomal(uint8_t* packet, int packetLen,
		const sockaddr_in clientAddr, int questionEndOffset);

	uint16_t get_orgID(uint8_t* packet);
	void cnv_orgID_To_ProxyID(uint8_t* packet, uint16_t proxyID);

	void create_ProxyID();
	
	DNSLogItem create_DNSLogItem(uint8_t* packet, CString domain,
		int QTYPEoffset, DnsLogResult dnsLogType);

	//CString get_queryType(uint8_t packet, )

	//void set_soc(SOCKET set);

private:
	SOCKET soc = NULL;
	SOCKET wsaSoc = NULL;
	sockaddr_in addr_Sendto = { 0 };
	DomainBlockModel* p_Model = nullptr;
	HANDLE h_Iocp = NULL;
	ProxyID_Store* p_ProxyID_Store = nullptr;

	uint16_t proxyID_cnt = 0;

	HWND h_View = nullptr;
};

