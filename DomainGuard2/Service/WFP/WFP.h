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

	void addFile(wfpFileInfo& info);
	DWORD addBlockFilter(const GUID& layer, FWP_BYTE_BLOB* appID,
		UINT64* filterID, const wfpFileInfo& info, ADDRESS_FAMILY addressFamily,
		bool isOut);
	void deleteBlockFilter(const CString filePath);

	

private:
	WCHAR init_Path[MAX_PATH]{};

	HANDLE h_engine = nullptr;
	WFPModel* p_WFPModel = nullptr;
};

