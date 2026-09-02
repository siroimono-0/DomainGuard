#include "pch.h"
#include "WFP.h"
#include <WS2tcpip.h>
#include <WinSock2.h>

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









