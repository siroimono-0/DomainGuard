#include "pch.h"
#include "WK_DNS.h"
#include "ProxyID_Store.h"
#include "../../Model/DomainBlockModel.h"

using namespace std;

WK_DNS::WK_DNS(SOCKET& soc, SOCKET& wsaSoc,
	sockaddr_in addr_Sendto, HANDLE h_iocp, DomainBlockModel* p_Model,
	ProxyID_Store* p_ProxyID_Store, HWND h_View)
{
	this->soc = soc;
	this->wsaSoc = wsaSoc;
	this->addr_Sendto = addr_Sendto;
	this->p_Model = p_Model;
	this->h_Iocp = h_iocp;
	this->p_ProxyID_Store = p_ProxyID_Store;
	this->h_View = h_View;
	this->start_Thread();
}

void WK_DNS::start_Thread()
{
	AfxBeginThread(this->th_Func, this);
	return;
}

UINT AFX_CDECL WK_DNS::th_Func(LPVOID vp)
{
	WK_DNS* p_this = (WK_DNS*)vp;
	p_this->run();
	TRACE(_T("\nClose WK_DNS Thread\n"));
	::PostMessage(p_this->h_View, WM_DELETE_WK_DNS_FROM_WK_DNS, NULL, NULL);
	return 0;
}

void WK_DNS::run()
{
	while (1)
	{
		uint8_t buf[4096] = { 0 };
		int bufSize = sizeof(buf);
		sockaddr_in clientAddr = { 0 };
		int clientAddrSize = sizeof(clientAddr);

		int recvLen = ::recvfrom(this->soc, (char*)buf, bufSize,
			0, (sockaddr*)&clientAddr, &clientAddrSize);
		if (strcmp((char*)buf, "stop") == 0)
		{
			break;
		}
		// ======================== proxyID 등록 ======================== 
		// ======================== proxyID 등록 ======================== 
		pendingContext _pendingContext = { 0 };
		_pendingContext.localAddr = clientAddr;
		uint16_t orgID = this->get_orgID(buf);
		_pendingContext.orgId = orgID;

		this->create_ProxyID();

		this->p_ProxyID_Store->emplace_map(this->proxyID_cnt, _pendingContext);

		// ======================== proxyID 등록 ======================== 
		// ======================== proxyID 등록 ======================== 
		//this->p_ProxyID_Store->emplace_map()

		if (recvLen == SOCKET_ERROR)
		{
			int err = ::WSAGetLastError();
			CString ERR;
			ERR.Format(_T("\nrecvfrom Err ___ %d\n"), err);
			OutputDebugString(ERR);
		}

		int questionEndOffset = 0;
		string domain = this->paser(buf, recvLen, questionEndOffset);
		int QTYPEoffset = questionEndOffset - 4;
		CString cs_domain;
		cs_domain.Format(_T("%S"), domain.c_str());

		if (domain.empty())
		{
			continue;
		}

		if (this->p_Model->isBlocked(cs_domain))
		{
			DNSLogItem item =
				this->create_DNSLogItem
				(buf, cs_domain, QTYPEoffset, DnsLogResult::Blocked);
			this->p_ProxyID_Store->emplace_un_mp(this->proxyID_cnt, item);
			this->sendto_Blocked(buf, recvLen, clientAddr, questionEndOffset);
			this->p_ProxyID_Store->erase_map(this->proxyID_cnt);
		}
		else
		{	
			DNSLogItem item =
				this->create_DNSLogItem
				(buf, cs_domain, QTYPEoffset, DnsLogResult::Allowed);
			this->p_ProxyID_Store->emplace_un_map_pending(this->proxyID_cnt, item);

			this->sendto_Nomal(buf, recvLen, clientAddr, questionEndOffset);
		}

	}
	return;
}


DNSLogItem WK_DNS::create_DNSLogItem(uint8_t* packet, CString domain,
	int QTYPEoffset, DnsLogResult dnsLogType)
{
	DNSLogItem item;
	::GetLocalTime(&item.requestTime);

	item.requestTick = GetTickCount64();

	item.domain = domain;

	if (dnsLogType == DnsLogResult::Blocked)
	{
		item.responeseTime = 0;
	}
	else if (dnsLogType == DnsLogResult::Allowed)
	{
		item.responeseTime = -1;
	}
	item.result = dnsLogType;

	uint16_t qType = 0;
	memcpy(&qType, packet + QTYPEoffset, sizeof(qType));

	qType = ntohs(qType);

	if (qType == 1)
	{
		item.queryType = "A";
	}
	else if (qType == 2)
	{
		item.queryType = "NS";
	}
	else if (qType == 5)
	{
		item.queryType = "CNAME";
	}
	else if (qType == 12)
	{
		item.queryType = "PTR";
	}
	else if (qType == 15)
	{
		item.queryType = "MX";
	}
	else if (qType == 16)
	{
		item.queryType = "TXT";
	}
	else if (qType == 28)
	{
		item.queryType = "AAAA";
	}
	else if (qType == 33)
	{
		item.queryType = "SRV";
	}
	else if (qType == 65)
	{
		item.queryType = "HTTPS";
	}
	else if (qType == 255)
	{
		item.queryType = "ANY";
	}

	return item;
}

void WK_DNS::create_ProxyID()
{
	for (int i = 0; i < 65536; i++)
	{
		this->proxyID_cnt++;
		if (this->p_ProxyID_Store->find_map(this->proxyID_cnt))
		{
			continue;
		}

		return;
	}
}

uint16_t WK_DNS::get_orgID(uint8_t* packet)
{
	uint16_t orgID = 0;
	memcpy(&orgID, packet, sizeof(orgID));

	return ntohs(orgID);
}

void WK_DNS::cnv_orgID_To_ProxyID(uint8_t* packet, uint16_t proxyID)
{
	uint16_t proxyID_cnv_Network = ntohs(proxyID);
	memcpy(packet, &proxyID_cnv_Network, sizeof(proxyID_cnv_Network));
	return;
}

void  WK_DNS::sendto_Blocked(const uint8_t* packet, int packetLen, const sockaddr_in clientAddr,
	int questionEndOffset)
{
	int DNS_HEADER_SIZE = 12;
	if (packet == NULL || packetLen < DNS_HEADER_SIZE
		|| questionEndOffset < DNS_HEADER_SIZE)
	{
		return;
	}

	vector<uint8_t> response(packet, packet + questionEndOffset);

	response[2] = (uint8_t)(0x80 | (response[2] & 0x78) | (response[2] & 0x01));
	response[3] = (uint8_t)(0x80 | 0x03);

	for (int i = 6; i < 12; i++)
	{
		response[i] = 0;
	}
	const char* send_buf = (const char*)response.data();

	::sendto(this->soc, send_buf, response.size(), 0,
		(const sockaddr*)&clientAddr, sizeof(clientAddr));

	return;
}

void  WK_DNS::sendto_Nomal(uint8_t* packet, int packetLen,
	const sockaddr_in clientAddr, int questionEndOffset)
{
	int DNS_HEADER_SIZE = 12;
	if (packet == NULL || packetLen < DNS_HEADER_SIZE
		|| questionEndOffset < DNS_HEADER_SIZE)
	{
		return;
	}

	ioContext* io = new ioContext;
	io->wsaBuf.buf = io->storage;
	io->wsaBuf.len = sizeof(io->storage);
	io->flag = 0;
	io->localAddr = clientAddr;
	io->requestId = ((uint16_t)packet[0] << 8) | (uint16_t)packet[1];

	int ret2 = ::WSARecv(this->wsaSoc, &io->wsaBuf, 1, nullptr, &io->flag, &io->ov, nullptr);
	if (ret2 == SOCKET_ERROR)
	{
		CString s;
		s.Format(_T("\nWSARecv ERR ___ %d\n"), ::WSAGetLastError());
		OutputDebugString(s);
	}

	this->cnv_orgID_To_ProxyID(packet, this->proxyID_cnt);

	::sendto(this->wsaSoc, (const char*)packet, packetLen, 0,
		(const sockaddr*)&this->addr_Sendto, sizeof(this->addr_Sendto));
	return;
}

std::string WK_DNS::paser(uint8_t* packet, int packetLen, int& questionEndOffset)
{
	int DNS_HEADER_SIZE = 12;

	if (packet == nullptr || packetLen <= DNS_HEADER_SIZE)
	{
		return string();
	}

	int offset = DNS_HEADER_SIZE;
	string domain;

	while (offset < packetLen)
	{
		uint8_t labelLen = packet[offset];
		if (packet[offset] == 0)
		{
			break;
		}

		if ((packet[offset] & 0xC0) != 0)
		{
			return string();
		}

		if (offset + packet[offset] >= packetLen)
		{
			return string();
		}

		for (int i = 1; i <= packet[offset]; i++)
		{
			domain += packet[offset + i];
		}

		offset += packet[offset] + 1;
		if (!domain.empty() && packet[offset] != 0)
		{
			domain += ".";
		}

	}
	questionEndOffset = offset + 4 + 1;
	return domain;
}



















