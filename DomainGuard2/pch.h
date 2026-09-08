// pch.h: 미리 컴파일된 헤더 파일입니다.
// 아래 나열된 파일은 한 번만 컴파일되었으며, 향후 빌드에 대한 빌드 성능을 향상합니다.
// 코드 컴파일 및 여러 코드 검색 기능을 포함하여 IntelliSense 성능에도 영향을 미칩니다.
// 그러나 여기에 나열된 파일은 빌드 간 업데이트되는 경우 모두 다시 컴파일됩니다.
// 여기에 자주 업데이트할 파일을 추가하지 마세요. 그러면 성능이 저하됩니다.

#ifndef PCH_H
#define PCH_H

// 여기에 미리 컴파일하려는 헤더 추가
#include "framework.h"
#include <afxstr.h>
#include <afxwin.h>
#include <afxmt.h>
#include <string>
#include <vector>
#include <unordered_map>

using namespace std;

#define TIMER_3SEC 1
#define TIMER_3SEC_DRIVERLOG 2
#define DOMAIN_MAX_LENGTH 253

constexpr UINT WM_QFINISHED_FROM_WK_SQLITE = WM_APP + 1;
constexpr UINT WM_CLICKED_FROM_CBLOCKLISTCTRL = WM_APP + 2;
//constexpr UINT WM_DELETE_FROM_CBLOCKLISTCTRL = WM_APP + 3;
constexpr UINT WM_CLICKED_FROM_CWFPLISTCTRL = WM_APP + 3;

constexpr LPARAM HINT_UPDATE_VEC_BLOCK_DOMAIN = WM_APP + 4;
constexpr LPARAM HINT_INSERT_BLOCK_DOMAIN = WM_APP + 5;
constexpr LPARAM HINT_DELETE_BLOCK_DOMAIN = WM_APP + 6;
constexpr LPARAM HINT_DELETE_WFP_CTRL = WM_APP + 7;

constexpr LPARAM HINT_UPDATE_DNSLOGITEMMODEL = WM_APP + 100;

constexpr LPARAM HINT_INSERT_WFPMODEL = WM_APP + 200;
constexpr LPARAM HINT_SELECTINIT_WFPMODEL = WM_APP + 201;

constexpr UINT DNS_BUFFER_SIZE = 65536;

enum class DirectionType
{
	both,
	out,
	in
};

enum class protocolType
{
	all,
	TCP,
	UDP
};

enum class clickedType {
	checkBox_on,
	checkBox_off,
	deleteBtn,
	wfp_DeleteBtn
};

enum class sqlCmp {
	nomal,
	reverse
};

enum class sqlType {
	select,
	insert,
	remove,
	update,
	select_Init,
	wfpInsert,
	wfpDelete,
	wfpSelect_Init
};

enum class initStat {
	init,
	nomal
};

typedef struct wfpFileInfo {
	CString filePath;
	DirectionType _DirectionType;
	protocolType _protocolType;
	CString ip;
	CString port;
	vector<UINT64> vec_ID;
	initStat _initStat = initStat::nomal;
}wfpFileInfo;

typedef std::vector<wfpFileInfo> vecWfpFileInfo;

typedef struct sql_Q_Job
{
	sqlType type;
	CString domain;
	bool block;
	wfpFileInfo wfpInfo;
}sql_Q_Job;

typedef struct blockDomain
{
	CString domain;
	bool block;
	//std::string domain_driver;
	//char  domain_Driver[DOMAIN_MAX_LENGTH];
	//int domain_Driver_Len = 0;
}blockDomain;



typedef std::vector<blockDomain> vecBlockDomain;

typedef struct Q_Finished_Data {

	sqlType sql_Type;
	vecBlockDomain vec_BlockDomain;
	blockDomain _blockDomain;
	wfpFileInfo info;
	vecWfpFileInfo vec_Info;

} Q_Finished_Data;

typedef struct ioContext {
	OVERLAPPED ov{};
	WSABUF wsaBuf{};
	char storage[DNS_BUFFER_SIZE];
	DWORD flag;
	sockaddr_in localAddr;

	uint16_t requestId{};
};

class wsaSocContext {
public:
	SOCKET soc = NULL;

private:
};

typedef struct pendingContext
{
	sockaddr_in localAddr;
	uint16_t orgId{};
}pendingContext;

class Hint_UpdateAllViews
{
public:
	CString domain;
	CString filePath;
};

enum class DnsLogResult
{
	Allowed,
	Blocked,
	Timeout,
	Error
};

typedef struct DNSLogItem
{
	SYSTEMTIME requestTime{};
	ULONGLONG requestTick;

	CString domain;
	CString queryType;
	DnsLogResult result;
	int responeseTime;
};

enum class DriverLogResult
{
	Allowed,
	Blocked,
	Timeout,
	Error
};

typedef struct DriverLogItem
{
	SYSTEMTIME requestTime{};
	ULONGLONG requestTick;

	CString domain;
	CString queryType;
	DriverLogResult result;
	int responeseTime;
};

typedef std::vector<DNSLogItem> vecDNSLogItem;
typedef std::unordered_map<uint16_t, DNSLogItem> un_map_DNSLogItem;
typedef std::vector<DriverLogItem> vecDriverLogItem;










#endif //PCH_H
