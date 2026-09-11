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
	vecDriverLogItem get_vecDriverLog_To_Doc();
	// ==================== Driver ==================== 
	// ==================== Driver ==================== 

	void close();
	void set_h_MainFrame(HWND h_MainFrame);
	void close_SQLITE();
	void close_DNS();
	void delete_WK_DNS_OK();
	void delete_WK_GQCS_DNS_OK();
	void delete_p_WK__p_WK_GQCS();


private:
	HWND h_MainFrame = nullptr;

	CDomainGuard2Doc* p_Doc = nullptr;
	SQLITE* p_SQLITE = nullptr;
	DNS* p_DNS = nullptr;
	WFP* p_WFP = nullptr;
	CalloutDriverClient* p_CalloutDriver = nullptr;

	bool isDelete_p_SQLITE_WK = false;
	bool isDelete_p_DNS_WK = false;
	bool isDelete_p_DNS_GQCS_WK = false;
};












