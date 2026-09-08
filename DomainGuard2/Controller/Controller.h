#pragma once
#include "pch.h"

class CDomainGuard2Doc;
class SQLITE;
class DNS;
class WFP;
class CalloutDriverClient;

class Controller
{
public:
	void set_p_Doc(CDomainGuard2Doc* p_Doc);
	void set_p_SQLITE(SQLITE* p_SQLITE);
	void set_p_DNS(DNS* p_DNS);
	void set_p_WFP(WFP* p_WFP);
	void set_p_CalloutDriver(CalloutDriverClient* p_CalloutDriver);

	void insert_To_SQLITE(CString domain);
	void wfp_Insert_To_SQLITE(wfpFileInfo info);
	void finished_ResultQ();
	vecBlockDomain get_vec_BlockDomain_To_Doc();

	void push_SQL_To_SQLITE(clickedType type, CString domain);

	void update_DNSLogItem();
	un_map_DNSLogItem get_un_map_DNSLogItem();

	vecWfpFileInfo get_vec_Info_To_Doc();

	// ==================== WFP ==================== 
	// ==================== WFP ==================== 
	void addFile_To_WFP(wfpFileInfo info);
	void deleteBlockFilter_To_WFP(const CString filePath);
	// ==================== WFP ==================== 
	// ==================== WFP ==================== 

	// ==================== Driver ==================== 
	// ==================== Driver ==================== 
	void test_Driver_Domain();
	void get_DriverLog_To_CalloutDriverClient();
	// ==================== Driver ==================== 
	// ==================== Driver ==================== 

private:

	CDomainGuard2Doc* p_Doc = nullptr;
	SQLITE* p_SQLITE = nullptr;
	DNS* p_DNS = nullptr;
	WFP* p_WFP = nullptr;
	CalloutDriverClient* p_CalloutDriver = nullptr;
};












