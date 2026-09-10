#pragma once

class CDomainGuard2Doc;
class CDomainGuard2View;
class Controller;
class SQLITE;
class ResultQ_SQLITE;
class DomainBlockModel;
class DNS;
class DNSLogItemModel;
class WFP;
class WFPModel;
class CalloutDriverClient;
class DriverLogModel;
class CMainFrame;

class AppContext
{
public:
	AppContext(CDomainGuard2Doc* p_Doc, CDomainGuard2View* p_View);
	~AppContext();

	void assembly();

	void start_Timer();

	void close(HWND h_MainFrame);
	void delete_ALL();
private:
	HWND h_MainFrame = nullptr;

	CDomainGuard2Doc* p_Doc;
	CDomainGuard2View* p_View;
	Controller* p_Controller;
	SQLITE* p_SQLITE;
	ResultQ_SQLITE* p_ResultQ_SQLITE = nullptr;
	DomainBlockModel* p_DomainBlockModel = nullptr;
	DNS* p_DNS = nullptr;
	DNSLogItemModel* p_DNSLogItemModel = nullptr;
	WFP* p_WFP = nullptr;
	WFPModel* p_WFPModel = nullptr;
	CalloutDriverClient* p_CalloutDriver = nullptr;
	DriverLogModel* p_DriverLogModel = nullptr;
};

