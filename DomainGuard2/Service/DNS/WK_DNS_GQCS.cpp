#include "pch.h"
#include "WK_DNS_GQCS.h"
#include "ProxyID_Store.h"

#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")

WK_DNS_GQCS::WK_DNS_GQCS(SOCKET& soc, SOCKET& wsaSoc,
	HANDLE h_Iocp, ProxyID_Store* p_ProxyID_Store)
{
	this->soc = soc;
	this->wsaSoc = wsaSoc;
	this->h_Iocp = h_Iocp;
	this->p_ProxyID_Store = p_ProxyID_Store;

	this->start_Thread();
	return;
}

void WK_DNS_GQCS::start_Thread()
{
	AfxBeginThread(this->th_Func, this);
	return;
}

UINT AFX_CDECL WK_DNS_GQCS::th_Func(LPVOID vp)
{
	WK_DNS_GQCS* p_this = (WK_DNS_GQCS*)vp;
	p_this->run();
	return 0;
}

void WK_DNS_GQCS::run()
{
	while (1)
	{
		DWORD recvLen = 0;
		ULONG_PTR key = 0;
		OVERLAPPED* p_ov = nullptr;
		bool ret = GetQueuedCompletionStatus(this->h_Iocp, &recvLen,
			&key, &p_ov, INFINITE);

		if (ret == false)
		{
			CString s;
			s.Format(_T("\nGetQueuedCompletionStatus ERR ___ %d\n"), ::WSAGetLastError());
			OutputDebugString(s);
		}
		wsaSocContext* p_cotext = (wsaSocContext*)key;
		ioContext* p_io = (ioContext*)p_ov;

		uint8_t* response =
			reinterpret_cast<uint8_t*>(p_io->wsaBuf.buf);

		uint16_t responseId =
			(static_cast<uint16_t>(response[0]) << 8) |
			static_cast<uint16_t>(response[1]);

		CString message;

		message.Format(
			_T("\nrequest(requestId = % u, responseId = % u,"
				"localPort = , recvLen = % lu\n"),
			p_io->requestId,
			responseId,
			ntohs(p_io->localAddr.sin_port),
			recvLen
		);

		OutputDebugString(message);

		uint8_t copyPacket[DNS_BUFFER_SIZE];
		memcpy(copyPacket, p_io->wsaBuf.buf, recvLen);

		uint16_t proxyID = this->get_ProxtID((uint8_t*)p_io->wsaBuf.buf);
		pendingContext _pendingContext = { 0 };
		if (this->p_ProxyID_Store->get_pendingContext(proxyID, _pendingContext) == false)
		{
			OutputDebugString(_T("\np_ProxyID_Store->get_pendingContext FALSE\n"));
			continue;
		}
		this->cnv_ProxyID_To_orgID(copyPacket, _pendingContext.orgId);

		int ret2 = ::sendto(this->soc, (const char*)copyPacket, recvLen, 0,
			(const sockaddr*)&_pendingContext.localAddr,
			sizeof(_pendingContext.localAddr));
		if (ret2 == SOCKET_ERROR)
		{
			CString s;
			s.Format(_T("GQCS sendto ERR ___ %d"), ::WSAGetLastError());
			OutputDebugString(s);
		}

		this->update_Allowed_DNSLogItem(proxyID);

		this->p_ProxyID_Store->erase_map(proxyID);

		delete p_io;
	}
	return;
}

void  WK_DNS_GQCS::cnv_ProxyID_To_orgID(uint8_t* packet, uint16_t orgID)
{
	uint16_t network_orgID = ntohs(orgID);
	memcpy(packet, &network_orgID, sizeof(network_orgID));
	return;
}

uint16_t WK_DNS_GQCS::get_ProxtID(uint8_t* packet)
{
	uint16_t ret = 0;
	memcpy(&ret, packet, sizeof(ret));
	return ntohs(ret);
}

void WK_DNS_GQCS::update_Allowed_DNSLogItem(uint16_t  proxyID)
{
	ULONGLONG responseTick = ::GetTickCount64();

	this->p_ProxyID_Store->update_un_mp(proxyID, responseTick);
	return;
}





