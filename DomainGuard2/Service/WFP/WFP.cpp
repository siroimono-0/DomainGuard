#include "pch.h"
#include "WFP.h"
#include <WS2tcpip.h>
#include <WinSock2.h>

#include "../../../SharedIoctl/SharedIoctl.h"
#include "../../Model/WFPModel.h"


WFP::WFP(WFPModel* p_WFPModel)
{
	this->p_WFPModel = p_WFPModel;
	this->init();
	return;
}

WFP::~WFP()
{
	if (this->h_engine != nullptr)
	{
		::FwpmEngineClose0(this->h_engine);
	}
	this->h_engine = nullptr;
	return;
}

void WFP::registerSniFilter()
{
	DWORD ret2 = ::FwpmTransactionBegin0(this->h_engine, 0);
	if (ret2 != ERROR_SUCCESS)
	{
		goto Abort;
	}

	DWORD ret_Callout_V4 =
		this->addSniCallout(GUID_Callout_V4,
			FWPM_LAYER_STREAM_V4,
			&this->sniCalloutId_V4);

	if (ret_Callout_V4 != ERROR_SUCCESS)
	{
		CString msg;
		msg.Format(L"V4 콜아웃 추가 실패: 0x%08X", ret_Callout_V4);
		AfxMessageBox(msg);

		goto Abort;
	}

	DWORD ret_Callout_V6 =
		this->addSniCallout(GUID_Callout_V6,
			FWPM_LAYER_STREAM_V6,
			&this->sniCalloutId_V6);

	if ( ret_Callout_V6 != ERROR_SUCCESS)
	{
		CString msg;
		msg.Format(L"V4 콜아웃 추가 실패: 0x%08X", ret_Callout_V6 );
		AfxMessageBox(msg);

		goto Abort;
	}

	DWORD ret_SniStreamFilter_V4 =
		this->addSniStreamFilter(
			GUID_Callout_V4,
			FWPM_LAYER_STREAM_V4,
			&this->sniStreamFilterId_V4);

	if (ret_SniStreamFilter_V4  != ERROR_SUCCESS)
	{
		CString msg;
		msg.Format(L"V4 콜아웃 추가 실패: 0x%08X", ret_SniStreamFilter_V4 );
		AfxMessageBox(msg);

		goto Abort;
	}

	DWORD ret_SniStreamFilter_V6 =
		this->addSniStreamFilter(
			GUID_Callout_V6,
			FWPM_LAYER_STREAM_V6,
			&this->sniStreamFilterId_V6);

	if (ret_SniStreamFilter_V6  != ERROR_SUCCESS)
	{
		CString msg;
		msg.Format(L"V6 콜아웃 추가 실패: 0x%08X", ret_SniStreamFilter_V6);
		AfxMessageBox(msg);

		goto Abort;
	}

	DWORD ret5 = ::FwpmTransactionCommit0(this->h_engine);
	if (ret5 != ERROR_SUCCESS)
	{
		goto Abort;
	}

	return;

Abort:
	::FwpmTransactionAbort0(this->h_engine);
	this->sniCalloutId_V4 = 0;
	this->sniCalloutId_V6 = 0;
	this->sniStreamFilterId_V4 = 0;
	this->sniStreamFilterId_V6 = 0;
	TRACE(
		L"SNI registration aborted\n"
	);
	return;
}

DWORD WFP::addSniCallout(const GUID GUID_Callout,
	const GUID GUID_Layer_Stream,
	UINT32* sniCalloutId)
{
	FWPM_CALLOUT0 callout{};
	callout.calloutKey = GUID_Callout;
	callout.applicableLayer = GUID_Layer_Stream;

	callout.displayData.name =
		const_cast<PWSTR>(
			L"DomainGuard HTTPS SNI Callout"
			);

	callout.displayData.description =
		const_cast<PWSTR>(
			L"Inspects TLS ClientHello SNI"
			);

	DWORD ret =
		::FwpmCalloutAdd0(
			this->h_engine,
			&callout,
			nullptr,
			sniCalloutId);

	if (ret != ERROR_SUCCESS)
	{
		TRACE(_T("addSniCallout ___ FwpmCalloutAdd0 ___ %d"),
			::GetLastError());
		return ret;
	}
	return ERROR_SUCCESS;
}

DWORD WFP::addSniStreamFilter(const GUID GUID_Callout,
	const GUID GUID_Layer_Stream,
	UINT64* sniStreamFilterId)
{
	FWPM_FILTER_CONDITION0 condition[2] = { 0 };
	condition[0].fieldKey = FWPM_CONDITION_IP_REMOTE_PORT;

	condition[0].matchType = FWP_MATCH_EQUAL;

	condition[0].conditionValue.type = FWP_UINT16;
	condition[0].conditionValue.uint16 = 443;

	condition[1].fieldKey = FWPM_CONDITION_DIRECTION;
	condition[1].matchType = FWP_MATCH_EQUAL;
	condition[1].conditionValue.type = FWP_UINT32;
	condition[1].conditionValue.uint32 = FWP_DIRECTION_OUTBOUND;

	FWPM_FILTER filter = { 0 };
	filter.displayData.name =
		const_cast<PWSTR>(
			L"DomainGuard HTTPS SNI Stream Filter"
			);

	filter.displayData.description =
		const_cast<PWSTR>(
			L"Sends outbound TLS traffic to SNI callout"
			);

	filter.layerKey = GUID_Layer_Stream;
	filter.subLayerKey = GUID_WFP;
	filter.numFilterConditions = ARRAYSIZE(condition);
	filter.filterCondition = condition;
	/*
	* 드라이버가 등록되지 않은 상태에서는
	* 인터넷 전체 차단으로 처리하지 않고 허용
	*/
	filter.flags = FWPM_FILTER_FLAG_PERMIT_IF_CALLOUT_UNREGISTERED;

	/*
	* 필터가 매칭되면 드라이버 콜아웃을 호출한다.
	*
	* 드라이버에서
	* FWPS_STREAM_ACTION_DROP_CONNECTION을 사용하려면
	* CALLOUT_UNKNOWN으로 등록해야 한다.
	*/
	filter.action.type = FWP_ACTION_CALLOUT_UNKNOWN;
	filter.action.calloutKey = GUID_Callout;

	filter.weight.type = FWP_EMPTY;

	DWORD ret = ::FwpmFilterAdd0(
		this->h_engine,
		&filter,
		nullptr,
		sniStreamFilterId);


	if (ret != ERROR_SUCCESS)
	{
		TRACE(
			L"FwpmFilterAdd0 SNI ERR: 0x%08X\n",
			ret
		);

		*sniStreamFilterId = 0;
		return ret;
	}

	TRACE(
		L"SNI stream filter registered: %llu\n",
		*sniStreamFilterId
	);

	return ERROR_SUCCESS;
}



void WFP::init()
{
	DWORD len = ::GetModuleFileNameW(
		nullptr, this->init_Path, ARRAYSIZE(this->init_Path)
	);

	if (len == 0 || len >= ARRAYSIZE(this->init_Path))
	{
		return;
	}

	FWPM_SESSION0 session = { 0 };
	session.displayData.name = _T("DomainGuard Dynamic Session");
	session.flags = FWPM_SESSION_FLAG_DYNAMIC;

	DWORD ret = ::FwpmEngineOpen0(
		NULL, RPC_C_AUTHN_WINNT, NULL,
		&session, &this->h_engine
	);

	if (ret != ERROR_SUCCESS)
	{
		TRACE(_T("\nFwpmEngineOpen0 ___ ERR ___ %d\n"), ret);
	}

	FWPM_SUBLAYER0 subLayer = { 0 };
	subLayer.subLayerKey = GUID_WFP;
	subLayer.displayData.name = _T("DomainGuard _ SubLayer");
	subLayer.flags = 0;
	subLayer.weight = 0x0100;

	DWORD ret2 = ::FwpmSubLayerAdd0(
		this->h_engine, &subLayer, nullptr
	);

	if (ret2 != ERROR_SUCCESS)
	{
		TRACE(_T("\nFwpmSubLayerAdd0 ___ ERR ___ %d\n"), ret2);
	}

	// ================= DNS 우회 차단 등록 ================= 
	// ================= DNS 우회 차단 등록 ================= 
	wfpFileInfo info;
	info.filePath = this->init_Path;
	info._DirectionType = DirectionType::out;
	info._protocolType = protocolType::UDP;
	info.ip = _T("ALL");
	info.port = _T("53");
	info._initStat = initStat::init;

	wfpFileInfo info2;
	info2.filePath = this->init_Path;
	info2._DirectionType = DirectionType::out;
	info2._protocolType = protocolType::TCP;
	info2.ip = _T("ALL");
	info2.port = _T("53");
	info2._initStat = initStat::init;

	this->addFile(info);
	this->addFile(info2);
	// ================= DNS 우회 차단 등록 ================= 
	// ================= DNS 우회 차단 등록 ================= 

	this->registerSniFilter();

	// V4, V6 콜아웃이 실제 커널에 등록되어 있는지 확인
	const GUID* keys[] = {
		&GUID_Callout_V4,
		&GUID_Callout_V6
	};

	for (int i = 0; i < 2; ++i)
	{
		FWPM_CALLOUT0* info = nullptr;

		DWORD ret = FwpmCalloutGetByKey0(
			this->h_engine, // 현재 WFP 엔진
			keys[i],        // 조회할 콜아웃 GUID
			&info           // 조회 결과를 받는 포인터
		);

		CString message;

		if (ret == ERROR_SUCCESS)
		{
			const bool registered =
				(info->flags & FWPM_CALLOUT_FLAG_REGISTERED) != 0;

			message.Format(
				L"SNI V%d: ID=%u, 커널 등록=%s",
				i == 0 ? 4 : 6,
				info->calloutId,
				registered ? L"YES" : L"NO"
			);

			// 조회 함수가 할당한 메모리를 해제
			FwpmFreeMemory0(reinterpret_cast<void**>(&info));
		}
		else
		{
			message.Format(
				L"SNI V%d: 조회 실패 0x%08X",
				i == 0 ? 4 : 6,
				ret
			);
		}

		AfxMessageBox(message);
	}
	return;
}

void WFP::deleteBlockFilter(const CString filePath)
{
	auto vec_info = this->p_WFPModel->get_vec_Info();
	vector<UINT64> vec_id;

	for (auto& v : vec_info)
	{
		if (v.filePath == filePath)
		{
			vec_id = v.vec_ID;
			break;
		}
	}

	for (auto& v : vec_id)
	{
		DWORD ret = ::FwpmFilterDeleteById0(this->h_engine, v);
		if (ret != ERROR_SUCCESS)
		{
			TRACE(_T("\n::FwpmFilterDeleteById0 ___ ERR ___ %d\n"), ret);
		}
	}

	return;
}
/*
	ref로 전달 -> wfp에서 infoID 채우고 vecblockID도 채움
	controll -> doc 전달 doc vec에 추가함
	doc model 추가생성 ui list로 차단목록 추가
	임의 ID생성해서 관리해야댐
	db저장
*/
void WFP::addFile(wfpFileInfo& info)
{
	FWP_BYTE_BLOB* appID = nullptr;

	DWORD ret = ::FwpmGetAppIdFromFileName0(info.filePath.GetString(), &appID);

	if (ret != ERROR_SUCCESS)
	{
		TRACE(_T("\nFwpmGetAppIdFromFileName0 ___ ERR ___ %d\n"), ret);
		return;
	}

	UINT64 v4ID_IN = 0;
	UINT64 v6ID_IN = 0;

	UINT64 v4ID_OUT = 0;
	UINT64 v6ID_OUT = 0;


	DWORD ret2 = ::FwpmTransactionBegin0(this->h_engine, 0);
	if (ret2 != ERROR_SUCCESS)
	{
		return;
	}

	if (info._DirectionType == DirectionType::in)
	{
		DWORD ret3 = this->addBlockFilter(FWPM_LAYER_ALE_AUTH_RECV_ACCEPT_V4,
			appID, &v4ID_IN, info, AF_INET, false);
		if (ret3 != ERROR_SUCCESS)
		{
			TRACE(_T("\naddBlockFilter ___ ERR ___ %d\n"), ret3);
			::FwpmTransactionAbort0(this->h_engine);
			FwpmFreeMemory0((void**)(&appID));
			return;
		}

		if (info.ip == _T("ALL"))
		{
			DWORD ret4 = this->addBlockFilter(FWPM_LAYER_ALE_AUTH_RECV_ACCEPT_V6,
				appID, &v6ID_IN, info, AF_INET6, false);
			if (ret4 != ERROR_SUCCESS)
			{
				TRACE(_T("\naddBlockFilter ___ ERR ___ %d\n"), ret4);
				::FwpmTransactionAbort0(this->h_engine);
				FwpmFreeMemory0((void**)(&appID));
				return;
			}
			info.vec_ID.push_back(v6ID_IN);
		}

		info.vec_ID.push_back(v4ID_IN);
	}
	else if (info._DirectionType == DirectionType::out)
	{
		DWORD ret3 = this->addBlockFilter(FWPM_LAYER_ALE_AUTH_CONNECT_V4,
			appID, &v4ID_OUT, info, AF_INET, true);
		if (ret3 != ERROR_SUCCESS)
		{
			TRACE(_T("\naddBlockFilter ___ ERR ___ %d\n"), ret3);
			::FwpmTransactionAbort0(this->h_engine);
			FwpmFreeMemory0((void**)(&appID));
			return;
		}

		if (info.ip == _T("ALL"))
		{
			DWORD ret4 = this->addBlockFilter(FWPM_LAYER_ALE_AUTH_CONNECT_V6,
				appID, &v6ID_OUT, info, AF_INET6, true);
			if (ret4 != ERROR_SUCCESS)
			{
				TRACE(_T("\naddBlockFilter ___ ERR ___ %d\n"), ret4);
				::FwpmTransactionAbort0(this->h_engine);
				FwpmFreeMemory0((void**)(&appID));
				return;
			}
			info.vec_ID.push_back(v6ID_OUT);
		}

		info.vec_ID.push_back(v4ID_OUT);
	}
	else if (info._DirectionType == DirectionType::both)
	{
		DWORD ret3 = this->addBlockFilter(FWPM_LAYER_ALE_AUTH_RECV_ACCEPT_V4,
			appID, &v4ID_IN, info, AF_INET, false);
		if (ret3 != ERROR_SUCCESS)
		{
			TRACE(_T("\naddBlockFilter ___ ERR ___ %d\n"), ret3);
			::FwpmTransactionAbort0(this->h_engine);
			FwpmFreeMemory0((void**)(&appID));
			return;
		}

		if (info.ip == _T("ALL"))
		{
			DWORD ret4 = this->addBlockFilter(FWPM_LAYER_ALE_AUTH_RECV_ACCEPT_V6,
				appID, &v6ID_IN, info, AF_INET6, false);
			if (ret4 != ERROR_SUCCESS)
			{
				TRACE(_T("\naddBlockFilter ___ ERR ___ %d\n"), ret4);
				::FwpmTransactionAbort0(this->h_engine);
				FwpmFreeMemory0((void**)(&appID));
				return;
			}
			info.vec_ID.push_back(v6ID_IN);
		}

		DWORD ret5 = this->addBlockFilter(FWPM_LAYER_ALE_AUTH_CONNECT_V4,
			appID, &v4ID_OUT, info, AF_INET, true);
		if (ret5 != ERROR_SUCCESS)
		{
			TRACE(_T("\naddBlockFilter ___ ERR ___ %d\n"), ret5);
			::FwpmTransactionAbort0(this->h_engine);
			FwpmFreeMemory0((void**)(&appID));
			return;
		}

		if (info.ip == _T("ALL"))
		{
			DWORD ret6 = this->addBlockFilter(FWPM_LAYER_ALE_AUTH_CONNECT_V6,
				appID, &v6ID_OUT, info, AF_INET6, true);
			if (ret6 != ERROR_SUCCESS)
			{
				TRACE(_T("\naddBlockFilter ___ ERR ___ %d\n"), ret6);
				::FwpmTransactionAbort0(this->h_engine);
				FwpmFreeMemory0((void**)(&appID));
				return;
			}
			info.vec_ID.push_back(v6ID_OUT);
		}


		info.vec_ID.push_back(v4ID_IN);
		info.vec_ID.push_back(v4ID_OUT);
	}


	DWORD ret5 = ::FwpmTransactionCommit0(this->h_engine);
	if (ret5 != ERROR_SUCCESS)
	{
		::FwpmTransactionAbort0(this->h_engine);
	}

	FwpmFreeMemory0((void**)(&appID));

}

DWORD WFP::addBlockFilter(const GUID& layer,
	FWP_BYTE_BLOB* appID, UINT64* filterID, const wfpFileInfo& info,
	ADDRESS_FAMILY addressFamily, bool isOut)
{
	FWPM_FILTER_CONDITION0 condition[4] = { 0 };
	int conditionCnt = 0;

	condition[conditionCnt].fieldKey = FWPM_CONDITION_ALE_APP_ID;
	if (info._initStat == initStat::init)
	{
		condition[conditionCnt].matchType = FWP_MATCH_NOT_EQUAL;
	}
	else
	{
		condition[conditionCnt].matchType = FWP_MATCH_EQUAL;
	}
	condition[conditionCnt].conditionValue.type = FWP_BYTE_BLOB_TYPE;
	condition[conditionCnt].conditionValue.byteBlob = appID;
	conditionCnt++;

	if (info._protocolType != protocolType::all)
	{
		condition[conditionCnt].fieldKey = FWPM_CONDITION_IP_PROTOCOL;
		condition[conditionCnt].matchType = FWP_MATCH_EQUAL;
		condition[conditionCnt].conditionValue.type = FWP_UINT8;

		if (info._protocolType == protocolType::TCP)
		{
			condition[conditionCnt].conditionValue.uint8 = (UINT8)(IPPROTO_TCP);
		}
		else if (info._protocolType == protocolType::UDP)
		{
			condition[conditionCnt].conditionValue.uint8 = (UINT8)(IPPROTO_UDP);
		}
		conditionCnt++;
	}

	UINT32 remoteAddressV4 = 0;
	FWP_BYTE_ARRAY16 remoteAddressV6{};

	if (info._initStat == initStat::init && info.ip == _T("ALL"))
	{
		condition[conditionCnt].fieldKey = FWPM_CONDITION_IP_REMOTE_ADDRESS;
		condition[conditionCnt].matchType = FWP_MATCH_NOT_EQUAL;

		if (addressFamily == AF_INET)
		{
			IN_ADDR addressV4{};
			CString loopBack = _T("127.0.0.1");
			int ret = ::InetPtonW(AF_INET, loopBack.GetString(), &addressV4);
			if (ret != 1)
			{
				return ERROR_INVALID_PARAMETER;
			}

			remoteAddressV4 = ::ntohl(addressV4.S_un.S_addr);

			condition[conditionCnt].conditionValue.type = FWP_UINT32;
			condition[conditionCnt].conditionValue.uint32 = remoteAddressV4;
		}
		else if (addressFamily == AF_INET6)
		{
			IN6_ADDR addressV6{};

			int ret = ::InetPtonW(
				AF_INET6,
				L"::1",
				&addressV6);

			if (ret != 1)
			{
				return ERROR_INVALID_PARAMETER;
			}

			memcpy(
				remoteAddressV6.byteArray16,
				&addressV6,
				sizeof(remoteAddressV6.byteArray16));

			condition[conditionCnt].conditionValue.type =
				FWP_BYTE_ARRAY16_TYPE;

			condition[conditionCnt].conditionValue.byteArray16 =
				&remoteAddressV6;
		}
		else
		{
			return ERROR_INVALID_PARAMETER;
		}

		conditionCnt++;
	}

	if (info.ip != _T("ALL"))
	{
		condition[conditionCnt].fieldKey = FWPM_CONDITION_IP_REMOTE_ADDRESS;
		condition[conditionCnt].matchType = FWP_MATCH_EQUAL;

		if (addressFamily == AF_INET)
		{
			IN_ADDR addressV4{};
			int ret = ::InetPtonW(AF_INET, info.ip.GetString(), &addressV4);
			if (ret != 1)
			{
				return ERROR_INVALID_PARAMETER;
			}

			remoteAddressV4 = ::ntohl(addressV4.S_un.S_addr);

			condition[conditionCnt].conditionValue.type = FWP_UINT32;
			condition[conditionCnt].conditionValue.uint32 = remoteAddressV4;
		}
		else if (addressFamily == AF_INET6)
		{
			IN6_ADDR addressV6{};
			int ret = ::InetPtonW(AF_INET6, info.ip.GetString(), &addressV6);
			if (ret != 1)
			{
				return ERROR_INVALID_PARAMETER;
			}

			memcpy(remoteAddressV6.byteArray16, &addressV6,
				sizeof(remoteAddressV6.byteArray16));

			condition[conditionCnt].conditionValue.type = FWP_BYTE_ARRAY16_TYPE;
			condition[conditionCnt].conditionValue.byteArray16 = &remoteAddressV6;
		}
		conditionCnt++;
	}

	if (info.port != _T("ALL"))
	{
		TCHAR* endPoint = nullptr;

		unsigned long portNum = _tcstoul(
			info.port.GetString(),
			&endPoint,
			10
		);

		if (info.port == endPoint ||
			*endPoint != _T('\0') ||
			portNum == 0 ||
			portNum > 65535)
		{
			return ERROR_INVALID_PARAMETER;
		}

		if (isOut)
		{
			condition[conditionCnt].fieldKey = FWPM_CONDITION_IP_REMOTE_PORT;
		}
		else
		{
			condition[conditionCnt].fieldKey = FWPM_CONDITION_IP_LOCAL_PORT;
		}

		condition[conditionCnt].matchType = FWP_MATCH_EQUAL;
		condition[conditionCnt].conditionValue.type = FWP_UINT16;
		condition[conditionCnt].conditionValue.uint16 = (UINT16)portNum;
		conditionCnt++;
	}

	FWPM_FILTER0 filter = { 0 };
	filter.displayData.name = _T("DomainGuard Filter");
	filter.layerKey = layer;
	filter.subLayerKey = GUID_WFP;
	filter.weight.type = FWP_EMPTY;
	filter.numFilterConditions = conditionCnt;
	filter.filterCondition = condition;
	filter.action.type = FWP_ACTION_BLOCK;

	return ::FwpmFilterAdd0(this->h_engine, &filter, NULL, filterID);
}









