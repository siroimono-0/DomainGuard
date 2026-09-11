#include "pch.h"
#include "DNS.h"
#include "WK_DNS.h"
#include "WK_DNS_GQCS.h"
#include "ProxyID_Store.h"

#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")

#include <shellapi.h>
#pragma comment(lib, "Shell32.lib")

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
	this->powershell_DNS_Setting_end();
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

	this->powershell_DNS_Setting_Begin();
	return;
}

bool DNS::powershell_DNS_Setting_Begin()
{
	WCHAR sysDir[MAX_PATH] = {};
	UINT len = ::GetSystemDirectory(sysDir, MAX_PATH);
	if (len == 0 || len >= MAX_PATH)
	{
		return false;
	}

	CString powershellPath(sysDir);
	powershellPath +=
		L"\\WindowsPowerShell\\v1.0\\powershell.exe";

	const WCHAR* arguments =
		L"-NoProfile -NonInteractive -Command \""
		L"try { "

		// 오류 발생 시 catch로 이동
		L"$ErrorActionPreference = 'Stop'; "

		// 연결된 일반 어댑터 중
		// IPv4 기본 게이트웨이가 있는 어댑터들을 조회
		L"$adapters = @(Get-NetIPConfiguration | "
		L"Where-Object { $_.IPv4DefaultGateway -ne $null }); "

		// 변경할 대상이 없으면 종료
		L"if ($adapters.Count -eq 0) { exit 2 }; "

		// 조회된 모든 대상 어댑터에 적용
		L"foreach ($adapter in $adapters) { "

		L"Set-DnsClientServerAddress "
		L"-InterfaceIndex $adapter.InterfaceIndex "
		L"-ServerAddresses '127.0.0.1' "
		L"-ErrorAction Stop; "

		L"}; "

		// 모두 성공
		L"exit 0 "

		L"} catch { "
		L"exit 1 "
		L"}\"";

	SHELLEXECUTEINFOW info = {};
	info.cbSize = sizeof(info);

	// 실행한 PowerShell의 프로세스 핸들을 받음
	info.fMask = SEE_MASK_NOCLOSEPROCESS;

	info.lpVerb = L"runas";  // 관리자 권한으로 실행
	info.lpFile = powershellPath.GetString();
	info.lpParameters = arguments;
	info.nShow = SW_HIDE;    // PowerShell 창 숨김(UAC 창은 표시될 수 있음)
	//info.nShow = SW_SHOW;    // PowerShell 창 숨김(UAC 창은 표시될 수 있음)

	if (!::ShellExecuteExW(&info))
	{
		DWORD error = ::GetLastError();

		// 관리자 권한 요청 취소 등
		TRACE(L"PowerShell 실행 실패: %lu\n", error);
		return false;
	}


	if (info.hProcess == nullptr)
		return false;

	// PowerShell 명령이 끝날 때까지 대기
	DWORD waitResult =
		::WaitForSingleObject(info.hProcess, INFINITE);

	DWORD exitCode = 1;
	BOOL gotExitCode = FALSE;

	if (waitResult == WAIT_OBJECT_0)
	{
		gotExitCode =
			::GetExitCodeProcess(info.hProcess, &exitCode);
	}

	::CloseHandle(info.hProcess);

	return gotExitCode && exitCode == 0;
}

bool DNS::powershell_DNS_Setting_end()
{
	WCHAR sysDir[MAX_PATH] = {};
	UINT len = ::GetSystemDirectory(sysDir, MAX_PATH);
	if (len == 0 || len >= MAX_PATH)
	{
		return false;
	}

	CString powershellPath(sysDir);
	powershellPath +=
		L"\\WindowsPowerShell\\v1.0\\powershell.exe";

	const WCHAR* arguments =
		L"-NoProfile -NonInteractive -Command \""
		L"try { "

		// 오류 발생 시 catch로 이동
		L"$ErrorActionPreference = 'Stop'; "

		// 연결된 일반 어댑터 중
		// IPv4 기본 게이트웨이가 있는 어댑터들을 조회
		L"$adapters = @(Get-NetIPConfiguration | "
		L"Where-Object { $_.IPv4DefaultGateway -ne $null }); "

		// 변경할 대상이 없으면 종료
		L"if ($adapters.Count -eq 0) { exit 2 }; "

		// 조회된 모든 대상 어댑터에 적용
		L"foreach ($adapter in $adapters) { "

		L"Set-DnsClientServerAddress "
		L"-InterfaceIndex $adapter.InterfaceIndex "
		L"-ServerAddresses '8.8.8.8' "
		L"-ErrorAction Stop; "

		L"}; "

		// 모두 성공
		L"exit 0 "

		L"} catch { "
		L"exit 1 "
		L"}\"";

	SHELLEXECUTEINFOW info = {};
	info.cbSize = sizeof(info);

	// 실행한 PowerShell의 프로세스 핸들을 받음
	info.fMask = SEE_MASK_NOCLOSEPROCESS;

	info.lpVerb = L"runas";  // 관리자 권한으로 실행
	info.lpFile = powershellPath.GetString();
	info.lpParameters = arguments;
	info.nShow = SW_HIDE;    // PowerShell 창 숨김(UAC 창은 표시될 수 있음)

	if (!::ShellExecuteExW(&info))
	{
		DWORD error = ::GetLastError();

		// 관리자 권한 요청 취소 등
		TRACE(L"PowerShell 실행 실패: %lu\n", error);
		return false;
	}


	if (info.hProcess == nullptr)
		return false;

	// PowerShell 명령이 끝날 때까지 대기
	DWORD waitResult =
		::WaitForSingleObject(info.hProcess, INFINITE);

	DWORD exitCode = 1;
	BOOL gotExitCode = FALSE;

	if (waitResult == WAIT_OBJECT_0)
	{
		gotExitCode =
			::GetExitCodeProcess(info.hProcess, &exitCode);
	}

	::CloseHandle(info.hProcess);

	return gotExitCode && exitCode == 0;
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
