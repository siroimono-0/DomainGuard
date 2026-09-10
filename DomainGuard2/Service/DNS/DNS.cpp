#include "pch.h"
#include "DNS.h"
#include "WK_DNS.h"
#include "WK_DNS_GQCS.h"
#include "ProxyID_Store.h"

#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")

DNS::DNS(DomainBlockModel* p_DomainBlockModel, HWND h_View)
{
	this->h_View = h_View;
	this->p_DomainBlockModel = p_DomainBlockModel;
	this->p_ProxyID_Store = new ProxyID_Store();

	WSAStartup(MAKEWORD(2, 2), &this->wsaData);
	this->soc = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	struct sockaddr_in addr = { 0 };
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	addr.sin_port = htons(53);

	int ret = ::bind(this->soc, (const SOCKADDR*)&addr, sizeof(addr));

	if (ret == SOCKET_ERROR)
	{
		CString s;
		s.Format(_T("\nbind ERR ___ %d\n"), ::WSAGetLastError());
		OutputDebugString(s);
	}

	this->create_WK();
	return;
}

DNS::~DNS()
{
	return;
}

void  DNS::delete_p_WK__p_WK_GQCS()
{
	delete this->p_WK;
	delete this->p_WK_GQCS;
	return;
}

void DNS::create_WK()
{
	this->h_Iocp = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

	this->wsaSoc = ::WSASocket(AF_INET, SOCK_DGRAM,
		IPPROTO_UDP, nullptr, 0, WSA_FLAG_OVERLAPPED);
	sockaddr_in addr_Sendto = { 0 };
	addr_Sendto.sin_family = AF_INET;
	addr_Sendto.sin_port = htons(53);
	inet_pton(AF_INET, "8.8.8.8", &addr_Sendto.sin_addr);

	int ret = ::connect(this->wsaSoc, (const sockaddr*)&addr_Sendto, sizeof(addr_Sendto));

	if (ret == SOCKET_ERROR)
	{
		CString s;
		s.Format(_T("\nconnect ERR ___ %d\n"), ::WSAGetLastError());
		OutputDebugString(s);
	}

	wsaSocContext* context = new wsaSocContext();
	context->soc = this->wsaSoc;
	::CreateIoCompletionPort((HANDLE)this->wsaSoc, this->h_Iocp, (ULONG_PTR)context, 0);

	this->p_WK = new WK_DNS(this->soc, this->wsaSoc,
		addr_Sendto, this->h_Iocp, this->p_DomainBlockModel, this->p_ProxyID_Store, this->h_View);

	this->p_WK_GQCS = new WK_DNS_GQCS(this->soc, this->wsaSoc, this->h_Iocp,
		 this->p_ProxyID_Store, this->h_View);
	return;
}

un_map_DNSLogItem DNS::get_un_map_DNSLogItem()
{
	return std::move(this->p_ProxyID_Store->get_un_map());
}

void DNS::close()
{
	char buf[6] = "stop\0";
	sockaddr_in addr = { 0 };
	addr.sin_family = AF_INET;
	addr.sin_port = htons(53);
	::InetPtonA(AF_INET, "127.0.0.1", &addr.sin_addr);
	::sendto(this->soc, buf, 6, 0, (const sockaddr*)&addr, sizeof(addr));

	::PostQueuedCompletionStatus(this->h_Iocp, 0, 0, nullptr);
	return;
}
