#pragma once

#include <Windows.h>
#include <fwpmu.h>
#include <afxstr.h>

#pragma comment(lib, "Fwpuclnt.lib")
#pragma comment(lib, "Rpcrt4.lib")

class WFPModel;

class WFP
{
public:
	WFP(WFPModel* p_WFPModel);
	~WFP();

	void init();
	void registerSniFilter();

	DWORD addSniCallout(const GUID GUID_Callout,
		const GUID GUID_Layer_Stream,
		UINT32* sniCalloutId);

	DWORD addSniStreamFilter(const GUID GUID_Callout,
		const GUID GUID_Layer_Stream,
		UINT64* sniStreamFilterId);

	void addFile(wfpFileInfo& info);
	DWORD addBlockFilter(const GUID& layer, FWP_BYTE_BLOB* appID,
		UINT64* filterID, const wfpFileInfo& info, ADDRESS_FAMILY addressFamily,
		bool isOut);
	void deleteBlockFilter(const CString filePath);

	

private:
	WCHAR init_Path[MAX_PATH]{};
	UINT32 sniCalloutId_V4 = 0;
	UINT32 sniCalloutId_V6 = 0;
	UINT64 sniStreamFilterId_V4 = 0;
	UINT64 sniStreamFilterId_V6 = 0;

	HANDLE h_engine = nullptr;
	WFPModel* p_WFPModel = nullptr;

};

